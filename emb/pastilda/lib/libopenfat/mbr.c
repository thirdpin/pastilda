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

/* Master boot record.
 */

#include <stdint.h>

#include "openfat/leaccess.h"
#include "openfat/blockdev.h"
#include "openfat/mbr.h"

/* In fat_core.c */
extern uint8_t _fat_sector_buf[];

static int mbr_read_sectors(const struct block_device *dev, 
			uint32_t sector, uint32_t count, void *buf)
{
	struct block_mbr_partition *part = (void*)dev;

	return block_read_sectors(part->whole, 
				part->first_lba + sector, count, buf);
}

static int mbr_write_sectors(const struct block_device *dev, 
			uint32_t sector, uint32_t count, const void *buf)
{
	struct block_mbr_partition *part = (void*)dev;

	return block_write_sectors(part->whole, 
				part->first_lba + sector, count, buf);
}

int mbr_partition_init(struct block_mbr_partition *part, 
			struct block_device *whole, uint8_t part_index)
{
	struct mbr_partition *part_table = (void*)&_fat_sector_buf[446];
	/* Read MBR from whole device */
	if(block_read_sectors(whole, 0, 1, _fat_sector_buf) != 1)
		return -1;
	
	part->whole = whole;

	part->first_lba = __get_le32(&part_table[part_index].first_lba);
	part->sector_count = __get_le32(&part_table[part_index].sector_count);

	part->bldev.get_sector_size = whole->get_sector_size;
	part->bldev.read_sectors = mbr_read_sectors;
	part->bldev.write_sectors = mbr_write_sectors;

	return 0;
}


