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

/* FAT Directory entry definitions.
 */
#ifndef __DIRENTRY_H
#define __DIRENTRY_H

#include <stdint.h>

struct fat_sdirent {
	char name[11]; /* DOS file name 8.3 */
	uint8_t attr;
	uint8_t Reserved;
	uint8_t create_time_fine;
	uint16_t create_time;
	uint16_t create_date;
	uint16_t access_date;
	uint16_t cluster_hi;
	uint16_t write_time;
	uint16_t write_date;
	uint16_t cluster_lo;
	uint32_t size;
} __attribute__((packed));


#define FAT_LAST_LONG_ENTRY 0x40

/* W95 long file name entries.  Characters are in UTF-16. */
struct fat_ldirent {
	uint8_t ord;
	uint16_t name1[5];
	uint8_t attr;
	uint8_t type;
	uint8_t checksum;
	uint16_t name2[6];
	uint16_t cluster_lo;
	uint16_t name3[2];
} __attribute__((packed));

uint8_t _fat_dirent_chksum(uint8_t *dosname);
int _fat_dir_seek_empty(struct fat_file_handle *dir, int entries);

#endif

