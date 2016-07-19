/*
 * This file is part of the pastilda project.
 * hosted at http://github.com/thirdpin/pastilda
 *
 * Copyright (C) 2016  Third Pin LLC
 * Written by Anastasiia Lazareva <a.lazareva@thirdpin.ru>
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

#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <string.h>

extern "C"
{
#include "blockdev.h"
#include "openfat.h"
#include "mbr.h"
}

#include "spi_ext.h"
#include "SST25.h"

using namespace SPI_CPP_Extension;

#define WBVAL(x) ((x) & 0xFF), (((x) >> 8) & 0xFF)
#define QBVAL(x) ((x) & 0xFF), (((x) >> 8) & 0xFF), (((x) >> 16) & 0xFF), (((x) >> 24) & 0xFF)

constexpr uint16_t FAKE_SECTOR_SIZE				= 512;
constexpr uint16_t FAKE_SECTOR_COUNT            = 16384; //(MEMORY_SIZE / FAKE_SECTOR_SIZE);
constexpr uint16_t BOOT_SIGNATURE               = 0x55AA;

constexpr uint16_t BYTES_PER_SECTOR				= 512;
constexpr uint16_t SECTORS_PER_CLUSTER			= 4;
constexpr uint16_t RESERVED_SECTORS				= 1;
constexpr uint16_t FAT_COPIES		    		= 2;
constexpr uint16_t ROOT_ENTRIES					= 512;

constexpr uint16_t BOOT_SIZE    				= 512;
constexpr uint16_t BOOT_SECTOR  				= 0;
constexpr uint16_t BOOT_SECTOR_COUNT  			= 1;

constexpr uint16_t FAT_SIZE    					= 4096;
constexpr uint16_t FAT_SECTOR_COUNT   			= 8;
constexpr uint16_t FAT1_SECTOR  				= 1;
constexpr uint16_t FAT2_SECTOR  				= 1 + FAT_SECTOR_COUNT;

constexpr uint16_t ROOT_SIZE           			= 16384;
constexpr uint16_t ROOT_SECTOR_COUNT   			= 32;

typedef enum {
	FAT_READY,
	FAT_ERROR
}FatState;

class FlashMemory
{
public:
	FlashMemory();
	static uint16_t get_sector_size(const struct block_device *dev);
	static int read_sectors(const struct block_device *dev, uint32_t sector, uint32_t count, void *buf);
	static int write_sectors(const struct block_device *dev, uint32_t sector, uint32_t count, const void *buf);

	void erase_chip();

	static int flash_read(uint32_t lba, uint8_t *copy_to);
	static int flash_write(uint32_t lba, const uint8_t *copy_from);
	static int flash_blocks(void);

	void print_tree();

private:
	SST25 *_sst25;
	SPI_ext *_spi;

	struct block_device dev;
	struct block_mbr_partition part;
	struct fat_vol_handle vol;
	struct fat_file_handle dir;
	struct fat_file_handle file;

	FatState _get_fat_state();
	void _set_fat_system_region();
	void _set_boot_region();
	void _set_fat_region();
	void _set_root_dir_region();
};

#endif
