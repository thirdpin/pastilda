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

#include <fs/file_system.h>

FileSystem *fs_pointer;

FileSystem::FileSystem()
{
	fs_pointer = this;
	_sst25.disable_write_protection();

	uint16_t res = _sst25.read_id();
	disk_set_callbacks(access_memory);

	FatState state = get_fat_state();
	if (state == FatState::FAT_ERROR) {
		format_to_FAT12();
	}
	f_mount(&FATFS_Obj, "0", 1);
}

int FileSystem::access_memory(MemoryCommand cmd, uint32_t sector, uint32_t count, uint8_t *copy_to, const uint8_t *copy_from)
{
	switch(cmd)
	{
		case READ:
			for(int i = 0; i < count; i++) {
				fs_pointer->_sst25.read_sector(sector, &copy_to[FAKE_SECTOR_SIZE * i]);
				sector++;
			}
			break;
		case WRITE:
			for(int i = 0; i < count; i++) {
				fs_pointer->_sst25.write_sector(sector, copy_from);
				sector++;
			}
			break;
	}

	return (count);
}

void FileSystem::erase_chip()
{
	_sst25.erase_full_chip();
}

int FileSystem::msd_read(uint32_t lba, uint8_t *copy_to)
{
	memset(copy_to, 0, FAKE_SECTOR_SIZE);
    if (lba >= FAKE_SECTOR_COUNT) {
    	return (1);
    }
    else {
    	fs_pointer->_sst25.read_sector(lba, copy_to);
    	return (0);
    }
}

int FileSystem::msd_write(uint32_t lba, const uint8_t *copy_from)
{
	if (lba >= FAKE_SECTOR_COUNT) {
		return (1);
	}
	else {
		fs_pointer->_sst25.write_sector(lba, copy_from);
		return (0);
	}
}
int FileSystem::msd_blocks(void)
{
	return (FAKE_SECTOR_COUNT);
}


FRESULT FileSystem::open_file_to_read(FIL *file, const char *name)
{
	return (f_open(file, name, FA_OPEN_EXISTING | FA_READ));
}
FRESULT FileSystem::open_file_to_write(FIL *file, const char *name)
{
	return (f_open(file, name, FA_CREATE_ALWAYS | FA_WRITE));
}
FRESULT FileSystem::close_file(FIL *file)
{
	return (f_close(file));
}
FRESULT FileSystem::read_next_file_chunk(FIL *file, void *buffer, uint32_t size)
{
	UINT nRead;
	return (f_read(file, buffer, size, &nRead));
}
FRESULT FileSystem::write_next_file_chunk(FIL *file, void *buffer, uint32_t size)
{
	UINT nWritten;
	return (f_write(file, buffer, size, &nWritten));
}
uint32_t FileSystem::get_file_tell(FIL *file)
{
	return (f_tell(file));
}

FRESULT FileSystem::read_file(FIL *file, const char *name, uint8_t *buffer)
{
	FRESULT result;
	UINT nRead;

	result = f_open(file, name, FA_OPEN_EXISTING | FA_READ);
	if (result == FR_OK)
	{
		result = f_read(file, buffer, file->fsize, &nRead);
	    f_close(file);
	}

	return (result);
}

FRESULT FileSystem::write_file(FIL *file, const char *name, uint8_t *buffer, uint32_t size)
{
	FRESULT result;
	UINT nWritten;

	result = f_open(file, name, FA_CREATE_ALWAYS | FA_WRITE);
	if (result == FR_OK)
	{
		f_write(file, buffer, size, &nWritten);
		f_close(file);
	}

	return (result);
}

FatState FileSystem::get_fat_state()
{
	uint16_t signature;
	uint8_t buf[BYTES_PER_SECTOR];

	access_memory(READ, MBR_SECTOR, MBR_SECTORS_COUNT, buf, 0);
	memcpy(&signature, &buf[SIGNATURE_OFFSET], sizeof(uint16_t));

	if (signature == SIGNATURE) {
		return (FatState::FAT_READY);
	}

	return (FatState::FAT_ERROR);
}

void FileSystem::format_to_FAT12()
{
	erase_chip();
	_set_master_boot_record();
	_set_FAT();
	_set_root_directory();
	scb_reset_system();
}
void FileSystem::_set_master_boot_record()
{
	Bpb_fat12_16 bpb;

	memcpy(bpb.bpb_base.jump_boot, JUMP_BOOT_VALUE, JUMP_BOOT_SIZE);
	memcpy(bpb.bpb_base.oem_name, OEM_NAME_VALUE, OEM_NAME_SIZE);
	bpb.bpb_base.bytes_per_sector = BYTES_PER_SECTOR;
	bpb.bpb_base.sectors_per_cluster = SECTORS_PER_CLUSTER;
	bpb.bpb_base.reserved_sectors_count = RESERVED_SECTORS;
	bpb.bpb_base.num_fats = FAT_COUNT;
	bpb.bpb_base.root_entry_count = ROOT_ENTRY_COUNT;
	bpb.bpb_base.total_sectors_fat16 = TOTAL_SECTORS_COUNT_FAT16;
	bpb.bpb_base.media = REMOVABLE_MEDIA;
	bpb.bpb_base.fat16_size = SECTORS_PER_FAT;
	bpb.bpb_base.sector_per_track = SECTORS_PER_TRACK;
	bpb.bpb_base. number_of_heads = NUMBER_OF_HEADS;
	bpb.bpb_base.count_of_hidden_sectors = HIDDEN_SECTORS_COUNT;
	bpb.bpb_base.total_sectors_fat32 = TOTAL_SECTORS_COUNT_FAT32;
	bpb.drive_number = DRIVE_NUMBER;
	bpb.reserved_1 = 0;
	bpb.boot_signature = BOOT_SIGNATURE;
	bpb.volume_serial_number = VOL_SERIAL_NUMBER;
	memcpy(bpb.volume_label, VOLUME_LABLE_VALUE, VOLUME_LABLE_SIZE);
	memcpy(bpb.filesystem_type, FS_TYPE_VALUE, FS_TYPE_SIZE);
	bpb.volume_label[VOLUME_LABLE_SIZE];
	bpb.filesystem_type[FS_TYPE_SIZE];

	Partition part;
	part.bootable = Bootable::PARTITION_ACTIVE;
	part.first_head = 0;
	part.first_sector = 0;
	part.first_cylinder = 0;
	part.type = PartitionType::FAT12;
	part.last_head = 0;
	part.last_sector = 0;
	part.last_cylinder = 0;
	part.first_lba = 0;
	part.sector_count = TOTAL_SECTORS_COUNT_FAT16;

	uint8_t boot_sector[BYTES_PER_SECTOR];
	memset(boot_sector, 0, BYTES_PER_SECTOR);
	memcpy(&boot_sector[BPB_OFFSET], &bpb, sizeof(Bpb_fat12_16));
	memcpy(&boot_sector[PARTITION_OFFSET], &part, sizeof(Partition));
	memcpy(&boot_sector[SIGNATURE_OFFSET], &SIGNATURE, sizeof(uint16_t));

	access_memory(WRITE, MBR_SECTOR, MBR_SECTORS_COUNT, 0, boot_sector);
}
void FileSystem::_set_FAT()
{
	uint8_t fat_sector[BYTES_PER_FAT];
	memset(fat_sector, 0, BYTES_PER_FAT);
	memcpy(fat_sector, FAT_HEADER, FAT_HEADER_SIZE);

	access_memory(WRITE, FAT1_SECTOR, FAT_SECTORS_COUNT, 0, fat_sector);
	access_memory(WRITE, FAT2_SECTOR, FAT_SECTORS_COUNT, 0, fat_sector);
}
void FileSystem::_set_root_directory()
{
	uint8_t root_sector[BYTES_PER_ROOT];
	memset(root_sector, 0, BYTES_PER_ROOT);
	access_memory(WRITE, ROOT_SECTOR, ROOT_SECTORS_COUNT, 0, root_sector);
}
