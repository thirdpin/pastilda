/*
 * This file is part of the pastilda project.
 * hosted at http://github.com/thirdpin/pastilda
 *
 * Copyright (C) 2016  Third Pin LLC
 *
 * Written by:
 *  Anastasiia Lazareva <a.lazareva@thirdpin.ru>
 *	Dmitrii Lisin <mrlisdim@ya.ru>
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

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <fs/fatfs/diskio.h>
#include <fs/fatfs/ff.h>
#include <fs/file_system_defines.h>
#include <libopencm3/cm3/scb.h>

class FileSystem
{
public:
	FATFS FATFS_Obj;
	FileSystem();

	static int access_memory(MemoryCommand cmd, uint32_t sector, uint32_t count, uint8_t *copy_to, const uint8_t *copy_from);
	void erase_chip();
	void format_to_FAT12();

	static int msd_read(uint32_t lba, uint8_t *copy_to);
	static int msd_write(uint32_t lba, const uint8_t *copy_from);
	static int msd_blocks(void);

	static FRESULT open_file_to_read(FIL *file, const char *name);
	static FRESULT open_file_to_write(FIL *file, const char *name);
	static FRESULT close_file(FIL *file);
	static FRESULT read_next_file_chunk(FIL *file, void *buffer, uint32_t size);
	static FRESULT write_next_file_chunk(FIL *file, void *buffer, uint32_t size);
	static uint32_t get_file_tell(FIL *file);
	static FRESULT read_file(FIL *file, const char *name, uint8_t *buffer);
	static FRESULT write_file(FIL *file, const char *name, uint8_t *buffer, uint32_t size);

private:
	SST25 _sst25;
	FatState get_fat_state();

	void _set_master_boot_record();
	void _set_FAT();
	void _set_root_directory();
};
#endif
