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

#include "flash_memory.h"

FlashMemory *flash_pointer;

FlashMemory::FlashMemory()
{
	flash_pointer = this;

	SPI_CPP_Extension::SPI_Conf sst25_conf = { 1, PA7, PA6, PA5 };
	_spi = new SPI_ext(sst25_conf);

	_spi->reset();
	_spi->disable();
	_spi->set_master_mode();
	_spi->set_baudrate_prescaler(BAUDRATE_FPCLK_DIV_2);
	_spi->set_standard_mode(MODE_0);
	_spi->set_data_drame_format(DFF_8BIT);
	_spi->set_bit_position(MSB_FIRST);
	_spi->enable_ss_output();
	_spi->set_software_slave_management(State::ENABLE);
	_spi->set_nss(HIGH);
	_spi->enable();

	_sst25 = new SST25(_spi, PA4);
	_sst25->disable_wtite_protection();

	FatState state = _get_fat_state();
	if (state == FatState::FAT_ERROR) {
		_set_fat_system_region();
	}
}

uint16_t FlashMemory::get_sector_size(const struct block_device *dev)
{
	(void)dev;
	return (FAKE_SECTOR_SIZE);
}

int FlashMemory::read_sectors(const struct block_device *dev, uint32_t sector, uint32_t count, void *buf)
{
	(void)dev;
	uint32_t end_sector = sector + count - 1;
	if (end_sector >= FAKE_SECTOR_COUNT) {
		return (0);
	}

	uint32_t real_start_sector = sector / 8;
	uint32_t real_end_sector = end_sector / 8;
	uint32_t real_sector_count = real_end_sector - real_start_sector + 1;
	uint32_t real_start_sector_part = (sector % 8);

	uint32_t data_size = real_sector_count * SECTOR_SIZE;
    uint8_t data[data_size];

	flash_pointer->_sst25->read_sectors(real_start_sector, real_sector_count, data);

	uint32_t start_address = real_start_sector_part * FAKE_SECTOR_SIZE;
	uint32_t end_address = start_address + (count * FAKE_SECTOR_SIZE);

	for (int i = start_address, j = 0; i < end_address; i++, j++) {
		((uint8_t*)buf)[j] = data[i];
	}

	return (count);
}

int FlashMemory::write_sectors(const struct block_device *dev, uint32_t sector, uint32_t count, const void *buf)
{
	(void)dev;
	uint32_t end_sector = (sector + count - 1);
	if (end_sector >= FAKE_SECTOR_COUNT) {
		return (0);
	}

	uint32_t real_start_sector = sector / 8;
	uint32_t real_end_sector = end_sector / 8;
	uint32_t real_sector_count = real_end_sector - real_start_sector + 1;
	uint32_t real_start_sector_part = (sector % 8);

	uint32_t copy_size = real_sector_count * SECTOR_SIZE;
	uint8_t sector_copy[copy_size];

	flash_pointer->_sst25->read_sectors(real_start_sector, real_sector_count, sector_copy);

	uint32_t start_address = real_start_sector_part * FAKE_SECTOR_SIZE;
	uint32_t end_address = start_address + (count * FAKE_SECTOR_SIZE);

	for (int i = start_address, j = 0; i < end_address; i++, j++) {
		sector_copy[i] = ((uint8_t*)buf)[j];
	}

	flash_pointer->_sst25->write_sectors(real_start_sector, real_sector_count, sector_copy);
	return (count);
}

int FlashMemory::flash_read(uint32_t lba, uint8_t *copy_to)
{
	memset(copy_to, 0, FAKE_SECTOR_SIZE);
    if (lba >= FAKE_SECTOR_COUNT) {
    	return (1);
    }
    else {
    	read_sectors(0, lba, 1, copy_to);
    	return (0);
    }
}

void FlashMemory::erase_chip()
{
	_sst25->erase_full_chip();
}
int FlashMemory::flash_write(uint32_t lba, const uint8_t *copy_from)
{
	if (lba >= FAKE_SECTOR_COUNT) {
		return (1);
	}
	else {
		write_sectors(0, lba, 1, copy_from);
		return (0);
	}
}
int FlashMemory::flash_blocks(void)
{
	return (FAKE_SECTOR_COUNT);
}
FatState FlashMemory::_get_fat_state()
{
	uint8_t buf[2];
	_sst25->read(510, 2, buf);
	uint16_t signature = ((buf[0] << 8) | buf[1]);
	if (signature == BOOT_SIGNATURE) {
		return (FatState::FAT_READY);
	}

	return (FatState::FAT_ERROR);
}
void FlashMemory::_set_fat_system_region()
{
	_sst25->erase_full_chip();
	_set_boot_region();
	_set_fat_region();
	_set_root_dir_region();
}

void FlashMemory::_set_boot_region()
{
	uint8_t copy_to[BOOT_SIZE];
	memset(copy_to, 0, BOOT_SIZE);

	uint8_t BootSector[] = {
		0xEB, 0x58, 0x90,					                // code to jump to the bootstrap code
		0x57, 0x49, 0x4E, 0x49, 0x4D, 0x41, 0x47, 0x45,     // OEM ID
		0x00, 0x02,				                            // bytes per sector
		0x08,					                            // sectors per cluster (0x04)
		0x01, 0x00,				                            // # of reserved sectors (1 boot sector)
		0x02,						                        // FAT copies (2)
		0x00, 0x02,					                        // root entries (512)
		0x00, 0x40,					                        // total number of sectors
		0xF8,							                    // media descriptor (0xF8 = Fixed disk)
		0x08, 0x00,						                    // sectors per FAT (16)
		0x00, 0x00,						                    // sectors per track (0)
		0x00, 0x00,						                    // number of heads (0)
		0x00, 0x00, 0x00, 0x00,					            // hidden sectors (0)
		0x00, 0x00, 0x00, 0x00,					            // large number of sectors (0)
		0x80,							                    // drive number (0)
		0x00,							                    // reserved
		0x29,							                    // extended boot signature
		0x10, 0x5C, 0xD1, 0x34,					            // volume serial number
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // volume label
		0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20	    // filesystem type
	};

	memcpy(copy_to, BootSector, sizeof(BootSector));
	copy_to[BOOT_SIZE - 2] = 0x55;
	copy_to[BOOT_SIZE - 1] = 0xAA;

	write_sectors(0, BOOT_SECTOR, BOOT_SECTOR_COUNT, copy_to);
}
void FlashMemory::_set_fat_region()
{
	uint8_t copy_to[FAT_SIZE];

	memset(copy_to, 0, FAT_SIZE);
	copy_to[0] = 0xF8;
	copy_to[1] = 0xFF;
	copy_to[2] = 0xFF;
	copy_to[3] = 0xFF;

	write_sectors(0, FAT1_SECTOR, FAT_SECTOR_COUNT, copy_to);
	write_sectors(0, FAT2_SECTOR, FAT_SECTOR_COUNT, copy_to);
}
void FlashMemory::_set_root_dir_region()
{
	uint8_t copy_to[ROOT_SIZE];
	memset(copy_to, 0, ROOT_SIZE);
	write_sectors(0, 17, ROOT_SECTOR_COUNT, copy_to);
}
