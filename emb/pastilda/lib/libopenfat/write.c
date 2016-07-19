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

/* FAT Filesystem write support implementation
 */
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "openfat.h"

#include "fat_core.h"

static uint32_t fat_find_free_cluster(struct fat_vol_handle *h)
{
	uint32_t i;

	for(i = h->last_cluster_alloc; i < h->cluster_count; i++) {
		if(_fat_get_next_cluster(h, i) == 0) {
			h->last_cluster_alloc = i;
			return i;
		}
	}

	for(i = 2; i < h->last_cluster_alloc; i++) {
		if(_fat_get_next_cluster(h, i) == 0) {
			h->last_cluster_alloc = i;
			return i;
		}
	}

	return 0; /* No free clusters */
}

static int 
fat32_set_next_cluster(const struct fat_vol_handle *h, uint8_t fat,
			uint32_t cluster, uint32_t next)
{
	uint32_t offset = cluster * 4;
	uint32_t sector;

	sector = h->reserved_sector_count + (fat * h->fat_size) +
		(offset / h->bytes_per_sector);
	offset %= h->bytes_per_sector;

	FAT_GET_SECTOR(h, sector);

	/* Preserve high nybble */
	next &= 0x0FFFFFFF;
	next |= __get_le32((uint32_t*)(_fat_sector_buf + offset)) & 0xF0000000;
	__put_le32((uint32_t*)(_fat_sector_buf + offset), next);

	FAT_PUT_SECTOR(h, sector);

	return 0;
}

static int 
fat16_set_next_cluster(const struct fat_vol_handle *h, uint8_t fat,
			uint16_t cluster, uint16_t next)
{
	uint32_t offset = cluster * 2;
	uint32_t sector;

	sector = h->reserved_sector_count + (fat * h->fat_size) + 
		(offset / h->bytes_per_sector);
	offset %= h->bytes_per_sector;

	FAT_GET_SECTOR(h, sector);
	__put_le16((uint16_t*)(_fat_sector_buf + offset), next);
	FAT_PUT_SECTOR(h, sector);

	return 0;
}

static int 
fat12_set_next_cluster(const struct fat_vol_handle *h, uint8_t fat,
			uint16_t cluster, uint16_t next)
{
	uint32_t offset = cluster + (cluster / 2);
	uint32_t sector;
	sector = h->reserved_sector_count + (fat * h->fat_size) + 
		(offset / h->bytes_per_sector);
	offset %= h->bytes_per_sector;

	FAT_GET_SECTOR(h, sector);
	if(offset == (uint32_t)h->bytes_per_sector - 1) {
		if(cluster & 1) {
			next <<= 4;
			_fat_sector_buf[offset] &= 0x0F;
			_fat_sector_buf[offset] |= next & 0xF0;
			FAT_PUT_SECTOR(h, sector);
			sector++;
			FAT_GET_SECTOR(h, sector);
			_fat_sector_buf[0] = next >> 8;
		} else {
			_fat_sector_buf[offset] = next & 0xFF;
			FAT_PUT_SECTOR(h, sector);
			sector++;
			FAT_GET_SECTOR(h, sector);
			_fat_sector_buf[0] &= 0xF0;
			_fat_sector_buf[0] |= (next >> 8) & 0x0F;
		}
	} else {
		if(cluster & 1) {
			next <<= 4;
			next |= __get_le16((uint16_t*)(_fat_sector_buf + offset)) & 0xF;
		} else {
			next &= 0x0FFF;
			next |= __get_le16((uint16_t*)(_fat_sector_buf + offset)) & 0xF000;
		}
		__put_le16((uint16_t*)(_fat_sector_buf + offset), next);
	}
	FAT_PUT_SECTOR(h, sector);

	return 0;
}

static int 
fat_set_next_cluster(const struct fat_vol_handle *h, 
			uint32_t cluster, uint32_t next)
{
	int ret = 0;
	for(int i = 0; i < h->num_fats; i++) {
		switch(h->type) {
		case FAT_TYPE_FAT12:
			ret |= fat12_set_next_cluster(h, i, cluster, next);
			break;
		case FAT_TYPE_FAT16:
			ret |= fat16_set_next_cluster(h, i, cluster, next);
			break;
		case FAT_TYPE_FAT32:
			ret |= fat32_set_next_cluster(h, i, cluster, next);
			break;
		}
	}
	return ret;
}

static int32_t fat_alloc_next_cluster(struct fat_vol_handle *h, 
				uint32_t cluster, int clear)
{
	/* Return next if already allocated */
	uint32_t next = _fat_get_next_cluster(h, cluster);

	if(next != fat_eoc(h)) 
		return next;
	
	/* Find free cluster to link to */
	next = fat_find_free_cluster(h);

	if(!next) /* No more free clusters */
		return 0;

	/* Write end of chain marker in new cluster */
	fat_set_next_cluster(h, next, fat_eoc(h));
	/* Add new cluster to chain */
	fat_set_next_cluster(h, cluster, next);

	if(clear) {
		/* Zero new cluster */
		uint32_t sector = fat_first_sector_of_cluster(
				h, next);
		FAT_FLUSH_SECTOR();
		memset(_fat_sector_buf, 0, h->bytes_per_sector);
		for(int i = 0; i < h->sectors_per_cluster; i++) {
			/* How do we report failure here?
			 * The cluster has already been allocated.
			 */
			int discard = block_write_sectors(h->dev, sector + i, 1, 
						_fat_sector_buf); 
			(void)discard;
		}
	}

	return next;
}

int fat_file_sync(struct fat_file_handle *h)
{
	struct fat_sdirent *dirent;
	/* Update directory entry with new size */
	FAT_GET_SECTOR(h->fat, h->dirent_sector);
	dirent = (void*)&_fat_sector_buf[h->dirent_offset];
	__put_le32(&dirent->size, h->size);
	__put_le16(&dirent->cluster_hi, h->first_cluster >> 16);
	__put_le16(&dirent->cluster_lo, h->first_cluster & 0xFFFF);
	FAT_PUT_SECTOR(h->fat, h->dirent_sector);
	FAT_FLUSH_SECTOR();
	return 0;
}

#define MIN(x, y) (((x) < (y))?(x):(y))
int fat_write(struct fat_file_handle *h, const void *buf, int size)
{
	int i;
	uint32_t sector;
	uint16_t offset;

	if(!h->cur_cluster && size) {
		/* File was empty, allocate first cluster. */
		h->first_cluster = fat_find_free_cluster(h->fat);
		if(!h->first_cluster) 
			return 0;
		h->cur_cluster = h->first_cluster;
		/* Write end of chain marker in new cluster */
		fat_set_next_cluster(h->fat, h->cur_cluster, fat_eoc(h->fat));
		/* Directory entry will be updated with size after the
		 * file write is done. 
		 */
	}

	/* Don't write past end of FAT12/FAT16 root directory! */
	if(h->root_flag && ((h->position + size) > h->size))
		size = h->size - h->position;

	_fat_file_sector_offset(h, &sector, &offset);

	for(i = 0; i < size; ) {
		uint16_t chunk = MIN(h->fat->bytes_per_sector - offset, 
					size - i);
		if(chunk < h->fat->bytes_per_sector)
			FAT_GET_SECTOR(h->fat, sector);
		else
			FAT_FLUSH_SECTOR();

		memcpy(_fat_sector_buf + offset, buf + i, chunk);
		FAT_PUT_SECTOR(h->fat, sector);
		h->position += chunk;
		i += chunk;
		if((h->position % h->fat->bytes_per_sector) != 0) 
			/* we didn't write until the end of the sector... */
			break;
		offset = 0;
		sector++;
		if(h->root_flag) /* FAT12/16 isn't a cluster chain */
			continue;
		if((sector % h->fat->sectors_per_cluster) == 0) {
			/* Go to next cluster... */
			uint32_t next_cluster = fat_alloc_next_cluster(h->fat, 
						h->cur_cluster, h->size == 0);
			if(!next_cluster)
				break;
			h->cur_cluster = next_cluster;
			sector = fat_first_sector_of_cluster(h->fat, 
						h->cur_cluster);
		}
	}

	if(h->dirent_sector && (h->position > h->size)) {
		/* Update directory entry with new size */
		h->size = h->position;
		if(!(h->flags & O_ASYNC))
			fat_file_sync(h);
	}

	return i;
}

static int fat_chain_unlink(const struct fat_vol_handle *vol, uint32_t cluster)
{
	int ret = 0;
	while(cluster && (cluster != fat_eoc(vol))) {
		uint32_t next = _fat_get_next_cluster(vol, cluster); 
		ret |= fat_set_next_cluster(vol, cluster, 0);
		cluster = next;
	}
	return ret;
}

int fat_unlink(struct fat_vol_handle *vol, const char *name)
{
	struct fat_file_handle h;
	int ret;

	ret = fat_open(vol, name, 0, &h);
	if(ret)
		return ret;

	/* Don't try to unlink directories, use fat_rmdir() instead. */
	if(!h.dirent_sector)
		return -EISDIR;

	/* Free up cluster chain */
	fat_chain_unlink(vol, h.first_cluster); 

	/* Mark directory entry as deleted */
	FAT_GET_SECTOR(vol, h.dirent_sector);
	_fat_sector_buf[h.dirent_offset] = 0xE5;
	FAT_PUT_SECTOR(vol, h.dirent_sector);

	/* FIXME: Remove long name entries. */

	return 0;
}

/* Build a short name for a long name.  n is used if name is too long. */
static void build_short_name(uint8_t *sname, const char *name, int n)
{
	int i, j;

	memset(sname, ' ', 11);
	for(i = 0; (i < 8) && name[i] && (name[i] != '.'); i++) 
		sname[i] = toupper(name[i]);
	
	char *suffix = strrchr(name, '.');
	if(suffix) for(j = 1; (j < 4) && suffix[j]; j++) 
		sname[j+7] = toupper(suffix[j]);

	if(((i == 8) && (name[i] != '.')) ||
	   ((suffix - name) != i)) {
		if(i > 6) 
			i = 6;
		sname[i] = '~';
		sname[i+1] = '0' + (n % 10);
	}
}

/* Create a new zero-length file */
int _fat_dir_create_file(struct fat_vol_handle *vol, const char *name,
		uint8_t attr, struct fat_file_handle *file)
{
	/* Check if file already exists */
	if(!fat_open(vol, name, 0, file)) 
		return -1; /* File exists */

	/* Attempt to construct a short name for the file */
	uint8_t sname[12];
	sname[11] = 0; /* fat_open() needs terminating null */
	for(int i = 1; i < 10; i++) {
		build_short_name(sname, name, i);
		if(fat_open(vol, (char*)sname, 0, file)) 
			break; /* We have a winner */
		sname[0] = ' ';
	}

	if(sname[0] == ' ')
		return -1; /* Couldn't find a short name */

	/* Find usable space in parent directory */
	_fat_dir_seek_empty(&vol->cwd, (strlen(name) / 13) + 2);

	/* Create long name directory entries */
	struct fat_ldirent ld;
	int last = 1;
	memset(&ld, 0, sizeof(ld));
	ld.attr = FAT_ATTR_LONG_NAME;
	for(int i = strlen(name) / 13; i >= 0; i--) {
		ld.ord = i + 1;
		if(last) {
			ld.ord |= FAT_LAST_LONG_ENTRY;
			last = 0;
		}
		int j;
		for(j = 0; j < 5; j++) 
			__put_le16(&ld.name1[j], name[i*13 + j]);
		for(j = 0; j < 6; j++) 
			__put_le16(&ld.name2[j], name[i*13 + j + 5]);
		for(j = 0; j < 2; j++) 
			__put_le16(&ld.name3[j], name[i*13 + j + 11]);
		ld.checksum = _fat_dirent_chksum(sname);
		if(fat_write(&vol->cwd, &ld, sizeof(ld)) != sizeof(ld))
			return -1;
	}

	/* Create short name entry */
	struct fat_sdirent fatent;
	memset(&fatent, 0, sizeof(fatent));
	fatent.attr = attr;
	memcpy(&fatent.name, sname, 11);
	/* TODO: Insert timestamp */
	if(attr == FAT_ATTR_DIRECTORY) {
		/* Allocate a cluster for directories */
		uint32_t cluster = fat_find_free_cluster(vol);
		if(!cluster) 
			return -1;
		fat_set_next_cluster(vol, cluster, fat_eoc(vol));
		__put_le16(&fatent.cluster_hi, cluster >> 16);
		__put_le16(&fatent.cluster_lo, cluster & 0xFFFF);
	}
	if(fat_write(&vol->cwd, &fatent, sizeof(fatent)) != sizeof(fatent))
		return -1;

	return fat_open(vol, name, 0, file);
}

int fat_mkdir(struct fat_vol_handle *vol, const char *name)
{
	int ret;
	struct fat_file_handle dir;
	struct fat_sdirent fatent;
	ret = _fat_dir_create_file(vol, name, FAT_ATTR_DIRECTORY, &dir);
	if(ret) 
		return ret;

	FAT_FLUSH_SECTOR();
	/* Clear out cluster */
	memset(_fat_sector_buf, 0, vol->bytes_per_sector);
	uint32_t sector = fat_first_sector_of_cluster(vol, dir.first_cluster);
	for(int i = 0; i < vol->sectors_per_cluster; i++) 
		FAT_PUT_SECTOR(vol, sector + i);

	memset(&fatent, 0, sizeof(fatent));
	fatent.attr = FAT_ATTR_DIRECTORY;
	memset(fatent.name, ' ', 11);

	/* Create '.' entry */
	fatent.name[0] = '.';
	__put_le16(&fatent.cluster_hi, dir.first_cluster >> 16);
	__put_le16(&fatent.cluster_lo, dir.first_cluster & 0xFFFF);
	ret = fat_write(&dir, &fatent, sizeof(fatent));
	if(ret < 0) 
		return ret;

	/* Create '..' entry */
	fatent.name[1] = '.';
	if((!vol->cwd.root_flag) &&
	   (vol->fat32.root_cluster != vol->cwd.first_cluster)) {
		__put_le16(&fatent.cluster_hi, vol->cwd.first_cluster >> 16);
		__put_le16(&fatent.cluster_lo, vol->cwd.first_cluster & 0xFFFF);
	} else {
		fatent.cluster_hi = 0;
		fatent.cluster_lo = 0;
	}
	ret = fat_write(&dir, &fatent, sizeof(fatent));
	return (ret < 0) ? ret : 0;
}

int fat_create(struct fat_vol_handle *vol, const char *name, int flags,
		  struct fat_file_handle *file)
{
	int ret = _fat_dir_create_file(vol, name, FAT_ATTR_ARCHIVE, file);
	file->flags = flags;
	return ret;
}

