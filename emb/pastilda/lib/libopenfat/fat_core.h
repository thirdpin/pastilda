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

/* FAT Filesystem core implementation, private interface
 */
#ifndef __FAT_CORE_H
#define __FAT_CORE_H

#include "bpb.h"
#include "direntry.h"

extern uint8_t _fat_sector_buf[];

extern struct _fat_cache {
	/* Working sector buffer, use _fat_sector_buf by default. */
	uint8_t *buf;

	/* Sector and block device for current contents of buf. */
	const void *bldev;
	uint32_t sector;

	/* Non-zero if buffer is out-of-sync with the physical medium. */
	uint8_t dirty;
} _fat_cache;

static inline uint32_t
fat_eoc(const struct fat_vol_handle *fat) 
{
	switch (fat->type) {
	case FAT_TYPE_FAT12:
		return 0x0FF8;
	case FAT_TYPE_FAT16:
		return 0xFFF8;
	case FAT_TYPE_FAT32:
		return 0x0FFFFFF8;
	}
	return -1;
}

static inline uint32_t 
fat_first_sector_of_cluster(const struct fat_vol_handle *fat, uint32_t n)
{
	return ((n - 2) * fat->sectors_per_cluster) + fat->first_data_sector;
}

uint32_t 
_fat_get_next_cluster(const struct fat_vol_handle *h, uint32_t cluster);

void _fat_file_root(struct fat_vol_handle *fat, struct fat_file_handle *h);
void _fat_file_init(struct fat_vol_handle *fat, const struct fat_sdirent *, 
		struct fat_file_handle *h);

void _fat_file_sector_offset(struct fat_file_handle *h, uint32_t *sector,
			uint16_t *offset);

int _fat_dir_create_file(struct fat_vol_handle *vol, const char *name,
		uint8_t attr, struct fat_file_handle *file);

#define FAT_FLUSH_SECTOR() do {\
	if(_fat_cache.dirty) \
		if(block_write_sectors(_fat_cache.bldev, _fat_cache.sector, \
					1, _fat_sector_buf) != 1) \
			return -EIO; \
	_fat_cache.dirty = 0; \
} while(0)

#define FAT_GET_SECTOR(fat, sectorn)	do {\
	if((_fat_cache.bldev==(fat)->dev) && (_fat_cache.sector==(sectorn)))\
		break; \
\
	FAT_FLUSH_SECTOR(); \
\
	_fat_cache.bldev = (fat)->dev; \
	_fat_cache.sector = (sectorn); \
\
	if(block_read_sectors((fat)->dev, (sectorn), 1, _fat_sector_buf) != 1)\
		return -EIO; \
} while(0)

#define FAT_PUT_SECTOR(fat, sectorn)	do {\
	if((_fat_cache.bldev!=(fat)->dev) || (_fat_cache.sector!=(sectorn)))\
		FAT_FLUSH_SECTOR(); \
\
	_fat_cache.bldev = (fat)->dev; \
	_fat_cache.sector = (sectorn); \
	_fat_cache.dirty = 1; \
} while(0)


#endif

