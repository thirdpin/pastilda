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

/** \file mbr.h
 * \brief PC Disk Partition Table implementation.
 * This module provides interpretation of the PC Master Boot Record
 * partition table, and provides a block device implementation for
 * partitions given an implementation for the whole disk using wrapper
 * functions.
 */

#ifndef __MBR_H
#define __MBR_H

#include <stdint.h>

#include "blockdev.h"

struct mbr_partition {
	uint8_t bootable;
	uint8_t first_chs[3];
	uint8_t type;
	uint8_t last_chs[3];
	uint32_t first_lba;
	uint32_t sector_count;
} __attribute__((packed));

/** \brief Structure representing block device for a PC disk partition.
 * Don't access directly.  No public fields. */
struct block_mbr_partition {
	struct block_device bldev;
	struct block_device *whole;
	uint32_t first_lba;
	uint32_t sector_count;
};

/** \brief Initialise a partition block device.
 * \param part Pointer to partition block device to initialize.
 * \param whole Pointer to block device for whole media.
 * \param part_index Partition index (0-3).
 * \return 0 on success.
 */
int mbr_partition_init(struct block_mbr_partition *part, 
			struct block_device *whole, uint8_t part_index);

#endif

