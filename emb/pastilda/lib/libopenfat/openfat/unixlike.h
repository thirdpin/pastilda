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

/*
 * Unix-like API for FAT filesystem access.
 */

#ifndef __UNIXLIKE_H
#define __UNIXLIKE_H

#include <openfat.h>

#include <stdlib.h>

FatVol * ufat_mount(struct block_device *dev);
static inline void ufat_umount(FatVol *vol) { free(vol); }

FatFile * ufat_open(FatVol *vol, const char *path, int flags);
static inline void ufat_close(FatFile *file) { free(file); }

int ufat_stat(FatFile *file, struct stat *stat);

#define ufat_read fat_read
#define ufat_write fat_write

#define ufat_chdir fat_chdir
#define ufat_mkdir fat_mkdir

#endif

