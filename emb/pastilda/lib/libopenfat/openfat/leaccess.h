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

/* Macros for structure access.  Host system may be little or big endian,
 * and may not be capable of misaligned access.  Always use these for
 * access to the on-disk FAT structures.
 */

#ifndef __LEACCESS_H
#define __LEACCESS_H

#include <stdint.h>

static inline uint16_t __get_le16(const uint16_t *p)
{
	return *(uint8_t *)p + (*((uint8_t *)p + 1) << 8);
}

static inline void __put_le16(uint16_t *p, uint16_t v)
{
	*(uint8_t *)p = v & 0xff;
	*((uint8_t *)p + 1) = v >> 8;
}

static inline uint32_t __get_le32(const uint32_t *p)
{
	return __get_le16((uint16_t *)p) + 
		(uint32_t)(__get_le16((const uint16_t *)p + 1) << 16); 
}

static inline void __put_le32(uint32_t *p, uint32_t v)
{
	__put_le16((uint16_t *)p, v & 0xffff);
	__put_le16((uint16_t *)p + 1, v >> 16);
}

#ifdef __TEST__
#include <stdio.h>
#include <assert.h>
int main(void)
{
	const char testdata[] = "\x01\x23\x45\x67";
	printf("%08X\n", __get_le32((uint32_t *)testdata));
	assert(__get_le32((uint32_t *)testdata) == 0x67452301);
	return 0;
}
#endif

#endif

