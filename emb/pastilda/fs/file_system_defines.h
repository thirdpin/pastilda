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

#ifndef FILE_SYSTEM_DEFINES_H
#define FILE_SYSTEM_DEFINES_H

#include <fs/drv/SST25.h>
#include <string.h>
#include "spi_ext.h"

typedef enum
{
	READ,
	WRITE
}MemoryCommand;

struct UsbMemoryControlParams {
	const int block_count;
	int (*read_block_func)(uint32_t lba, uint8_t *copy_to);
	int (*write_block_func)(uint32_t lba, const uint8_t *copy_from);
};

struct FatMemoryControlParams {
	int (*access_memory_func)(MemoryCommand cmd, uint32_t sector, uint32_t count, void *buf);
};

constexpr uint16_t FAKE_SECTOR_COUNT                       = 2048;//(MEMORY_SIZE / FAKE_SECTOR_SIZE);

constexpr uint8_t JUMP_BOOT_SIZE 							= 3;
constexpr uint8_t JUMP_BOOT_VALUE[JUMP_BOOT_SIZE] 			= {0xEB, 0xFF, 0x90};
constexpr uint8_t OEM_NAME_SIZE 							= 8;
constexpr uint8_t OEM_NAME_VALUE[OEM_NAME_SIZE] 			= {'M','S','W','I','N','4','.','1'};
constexpr uint16_t BYTES_PER_SECTOR 						= 512;
constexpr uint8_t SECTORS_PER_CLUSTER 						= 8;
constexpr uint16_t RESERVED_SECTORS  						= 1;
constexpr uint8_t FAT_COUNT   								= 2;
constexpr uint16_t ROOT_ENTRY_COUNT 						= 512;
constexpr uint16_t TOTAL_SECTORS_COUNT_FAT16 				= 2048;//16384;
constexpr uint8_t REMOVABLE_MEDIA 							= 0xF8;
constexpr uint16_t SECTORS_PER_FAT 							= 1;
constexpr uint16_t SECTORS_PER_TRACK 						= 0;
constexpr uint16_t NUMBER_OF_HEADS 							= 0;
constexpr uint32_t HIDDEN_SECTORS_COUNT 					= 0;
constexpr uint32_t TOTAL_SECTORS_COUNT_FAT32 				= 0;
constexpr uint8_t DRIVE_NUMBER 								= 0x80;
constexpr uint8_t BOOT_SIGNATURE 							= 0x29;
constexpr uint32_t VOL_SERIAL_NUMBER 						= 0xC4B1CDCF;
constexpr uint8_t VOLUME_LABLE_SIZE 						= 11;
constexpr uint8_t VOLUME_LABLE_VALUE[VOLUME_LABLE_SIZE]   	= {'N','O',' ','N','A','M','E',' ',' ',' ',' '};
constexpr uint8_t FS_TYPE_SIZE                              = 8;
constexpr uint8_t FS_TYPE_VALUE[FS_TYPE_SIZE] 				= {'F','A','T',' ','1','2',' ',' '};
constexpr uint16_t SIGNATURE               					= 0xAA55;

constexpr uint8_t BPB_OFFSET 								= 0;
constexpr uint16_t PARTITION_OFFSET 						= 446;
constexpr uint16_t SIGNATURE_OFFSET 						= 510;

constexpr uint8_t MBR_SECTOR 								= 0;
constexpr uint8_t MBR_SECTORS_COUNT 						= 1;

constexpr uint8_t FAT1_SECTOR 								= 1;
constexpr uint8_t FAT_SECTORS_COUNT 						= 1;
constexpr uint16_t BYTES_PER_FAT 							= (FAT_SECTORS_COUNT * BYTES_PER_SECTOR);
constexpr uint8_t FAT2_SECTOR 								= (FAT1_SECTOR + FAT_SECTORS_COUNT);
constexpr uint8_t FAT_HEADER_SIZE 							= 4;
constexpr uint8_t FAT_HEADER[FAT_HEADER_SIZE] 				= {0xF8, 0xFF, 0xFF, 0xFF};

constexpr uint8_t ROOT_SECTOR 								= (FAT2_SECTOR + FAT_SECTORS_COUNT);
constexpr uint8_t ROOT_SECTORS_COUNT 						= 32;
constexpr uint16_t BYTES_PER_ROOT 							= (ROOT_SECTORS_COUNT * BYTES_PER_SECTOR);

constexpr uint8_t  NUMBER_OF_CHARS      					= 30;
constexpr uint8_t  NUMBER_OF_FILES      					= 50;

#pragma pack (push, 1)
typedef struct {
	uint8_t  jump_boot[JUMP_BOOT_SIZE];
	uint8_t  oem_name[OEM_NAME_SIZE];
	uint16_t bytes_per_sector;
	uint8_t  sectors_per_cluster;
	uint16_t reserved_sectors_count;
	uint8_t  num_fats;
	uint16_t root_entry_count;
	uint16_t total_sectors_fat16;
	uint8_t  media;
	uint16_t fat16_size;
	uint16_t sector_per_track;
	uint16_t number_of_heads;
	uint32_t count_of_hidden_sectors;
	uint32_t total_sectors_fat32;
}Bpb_base;

typedef struct {
	Bpb_base bpb_base;
	uint8_t  drive_number;
	uint8_t  reserved_1;
	uint8_t  boot_signature;
	uint32_t volume_serial_number;
	uint8_t  volume_label[VOLUME_LABLE_SIZE];
	uint8_t  filesystem_type[FS_TYPE_SIZE];
}Bpb_fat12_16;

typedef struct {
	uint8_t bootable;
	uint8_t first_head;
	uint8_t first_sector;
	uint8_t first_cylinder;
	uint8_t type;
	uint8_t last_head;
	uint8_t last_sector;
	uint8_t last_cylinder;
	uint32_t first_lba;
	uint32_t sector_count;
}Partition;
#pragma pack (pop)

typedef enum {
	FAT12            = 0x01,
	FAT16_UNDER_32MB = 0x04,
	FAT16            = 0x06,
	FAT32            = 0x0B,
	FAT32_LBA        = 0x0C
}PartitionType;

typedef enum {
	PARTITION_ACTIVE   = 0x80,
	PARTITION_INACTIVE = 0x00
}Bootable;

typedef enum {
	FAT_READY,
	FAT_ERROR
}FatState;

typedef enum : uint8_t {
	OK    = 0,
	ERROR = 1
}Result;

#endif
