/*
 * This file is part of the openfat project.
 *
 * Copyright (C) 2011  Department of Physics, University of Otago
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* FAT Filesystem core implementation
 */

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "openfat.h"

#include "openfat/blockdev.h"
#include "openfat/leaccess.h"
#include "bpb.h"
#include "fat_core.h"

/* Build time configuration */
#define MAX_SECTOR_SIZE 512

uint8_t _fat_sector_buf[MAX_SECTOR_SIZE];
struct _fat_cache _fat_cache;

int fat_vol_init(const struct block_device *dev, struct fat_vol_handle *h) 
{
	struct bpb_common *bpb = (void *)&_fat_sector_buf;

	memset(h, 0, sizeof(*h));
	h->dev = dev;
	
	FAT_GET_SECTOR(h, 0);

	h->type = fat_type(bpb);
	h->cluster_count = _bpb_cluster_count(bpb);
	h->bytes_per_sector = __get_le16(&bpb->bytes_per_sector);
	h->sectors_per_cluster = bpb->sectors_per_cluster;
	h->first_data_sector = _bpb_first_data_sector(bpb);
	h->reserved_sector_count = __get_le16(&bpb->reserved_sector_count);
	h->num_fats = bpb->num_fats;
	h->fat_size = _bpb_fat_size(bpb);
	h->last_cluster_alloc = 2;
	if(h->type == FAT_TYPE_FAT32) {
		struct bpb_fat32 *bpb32 = (void *)&_fat_sector_buf;
		h->fat32.root_cluster = __get_le32(&bpb32->root_cluster);
	} else {
		h->fat12_16.root_sector_count = _bpb_root_dir_sectors(bpb);
		h->fat12_16.root_first_sector = _bpb_first_data_sector(bpb) - 
					h->fat12_16.root_sector_count;
	}
	_fat_file_root(h, &h->cwd);

	return 0;
}

uint32_t _fat_get_next_cluster(const struct fat_vol_handle *h, uint32_t cluster)
{
	uint32_t offset;
	uint32_t sector;

	if(h->type == FAT_TYPE_FAT12)
		offset = cluster + (cluster / 2);
	else if(h->type == FAT_TYPE_FAT16)
		offset = cluster * 2;
	else if(h->type == FAT_TYPE_FAT32)
		offset = cluster * 4;

	sector = h->reserved_sector_count + (offset / h->bytes_per_sector);
	offset %= h->bytes_per_sector;

	FAT_GET_SECTOR(h, sector);

	if(h->type == FAT_TYPE_FAT12) {
		uint32_t next;
		if(offset == (uint32_t)h->bytes_per_sector - 1) {
			/* Fat entry is over sector boundary */
			next = _fat_sector_buf[offset];
			FAT_GET_SECTOR(h, sector + 1);
			next += _fat_sector_buf[0] << 8;
		} else {
			next = __get_le16((uint16_t*)(_fat_sector_buf + offset));
		}
		if(cluster & 1) 
			return next >> 4;
		else
			return next & 0xFFF;
	} else if(h->type == FAT_TYPE_FAT16) {
		return __get_le16((uint16_t*)(_fat_sector_buf + offset));
	} else if(h->type == FAT_TYPE_FAT32) {
		return __get_le32((uint32_t*)(_fat_sector_buf + offset)) & 0x0FFFFFFF;
	}
	/* We shouldn't get here... */
	return 0;
}

void _fat_file_root(struct fat_vol_handle *fat, 
		struct fat_file_handle *h)
{
	memset(h, 0, sizeof(*h));
	h->fat = fat;

	if(fat->type == FAT_TYPE_FAT32) {
		h->first_cluster = fat->fat32.root_cluster;
	} else {
		/* FAT12/FAT16 root directory */
		h->root_flag = 1;
		h->first_cluster = fat->fat12_16.root_first_sector;
		h->size = h->fat->fat12_16.root_sector_count * h->fat->bytes_per_sector;
	}
	h->cur_cluster = h->first_cluster;
}

void _fat_file_init(struct fat_vol_handle *fat, 
		const struct fat_sdirent *dirent,
		struct fat_file_handle *h)
{
	memset(h, 0, sizeof(*h));
	h->fat = fat;
	h->first_cluster = ((uint32_t)__get_le16(&dirent->cluster_hi) << 16) | 
			__get_le16(&dirent->cluster_lo);
	h->size = __get_le32(&dirent->size);
	h->cur_cluster = h->first_cluster;
}

off_t fat_lseek(struct fat_file_handle *h, off_t offset, int whence)
{
	h->cur_cluster = h->first_cluster;

	switch(whence) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		offset += h->position;
		break;
	case SEEK_END:
		offset += h->size;
		break;
	default:
		return -1;
	}

	if(h->size && ((uint32_t)offset > h->size))
		offset = h->size;

	h->position = offset;

	if(h->root_flag) { /* FAT12/16 root dir isn't a cluster chain */
		return h->position;
	}

	/* Iterate over cluster chain to find cluster */
	while(offset >= (h->fat->sectors_per_cluster * h->fat->bytes_per_sector)) {
		h->cur_cluster = _fat_get_next_cluster(h->fat, h->cur_cluster);
		offset -= h->fat->sectors_per_cluster * h->fat->bytes_per_sector;
	}

	return h->position;
}

void _fat_file_sector_offset(struct fat_file_handle *h, uint32_t *sector,
			uint16_t *offset)
{
	if(h->root_flag) {
		/* FAT12/FAT16 root directory */
		*sector = h->cur_cluster +
				(h->position / h->fat->bytes_per_sector);
	} else {
		*sector = fat_first_sector_of_cluster(h->fat, h->cur_cluster);
		*sector += (h->position / h->fat->bytes_per_sector) % 
					h->fat->sectors_per_cluster;
	}
	*offset = h->position % h->fat->bytes_per_sector;
}

#define MIN(x, y) (((x) < (y))?(x):(y))
int fat_read(struct fat_file_handle *h, void *buf, int size)
{
	int i;
	uint32_t sector;
	uint16_t offset;

	_fat_file_sector_offset(h, &sector, &offset);

	/* Don't read past end of file */
	if(h->size && ((h->position + size) > h->size))
		size = h->size - h->position;

	for(i = 0; i < size; ) {
		uint16_t chunk = MIN(h->fat->bytes_per_sector - offset, size - i);
		FAT_GET_SECTOR(h->fat, sector);
		memcpy(buf + i, _fat_sector_buf + offset, chunk);
		h->position += chunk;
		i += chunk;
		if((h->position % h->fat->bytes_per_sector) != 0) 
			/* we didn't read until the end of the sector... */
			break;
		offset = 0;
		sector++;
		if(h->root_flag) /* FAT12/16 isn't a cluster chain */
			continue;
		if((sector % h->fat->sectors_per_cluster) == 0) {
			/* Go to next cluster... */
			h->cur_cluster = _fat_get_next_cluster(h->fat, 
						h->cur_cluster);
			if(h->cur_cluster == fat_eoc(h->fat)) 
				return i;
			sector = fat_first_sector_of_cluster(h->fat, 
						h->cur_cluster);
		}
	}

	return i;
}

