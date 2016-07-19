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

#ifndef SST25_H
#define SST25_H

#include <string.h>

#include "spi_ext.h"
#include "gpio_ext.h"

using namespace SPI_CPP_Extension;

constexpr uint8_t OPCODE_READ                 	 = 0x03;    /* Read data bytes           3   0  >=1 */
constexpr uint8_t OPCODE_FAST_READ_DUAL_IO    	 = 0xBB;
constexpr uint8_t OPCODE_FAST_READ_DUAL_OUTPUT   = 0x3B;
constexpr uint8_t OPCODE_FAST_READ            	 = 0x0B;    /* Higher speed read         3   1  >=1 */
constexpr uint8_t OPCODE_SECTOR_ERASE            = 0x20;    /* 4Kb Sector erase          3   0   0  */
constexpr uint8_t OPCODE_BLOCK_ERASE_32K         = 0x52;    /* 32Kbit block Erase        3   0   0  */
constexpr uint8_t OPCODE_BLOCK_ERASE_64K         = 0xD8;    /* 64Kbit block Erase        3   0   0  */
constexpr uint8_t OPCODE_CHIP_ERASE              = 0xC7;    /* Chip erase                0   0   0  */
constexpr uint8_t OPCODE_CHIP_ERASE_ALT          = 0x60;    /* Chip erase (alternate)    0   0   0  */
constexpr uint8_t OPCODE_PAGE_PROGRAM            = 0x02;    /* Byte program              3   0   1  */
constexpr uint8_t OPCODE_PAGE_PROGRAM_DUAL       = 0xA2;    /* Auto address increment    3   0  >=2 */
constexpr uint8_t OPCODE_RDSR                 	 = 0x05;    /* Read status register      0   0  >=1 */
constexpr uint8_t OPCODE_EWSR                 	 = 0x50;    /* Write enable status       0   0   0  */
constexpr uint8_t OPCODE_WRSR                 	 = 0x01;    /* Write Status Register     0   0   1  */
constexpr uint8_t OPCODE_WREN                 	 = 0x06;    /* Write Enable              0   0   0  */
constexpr uint8_t OPCODE_WRDI                 	 = 0x04;    /* Write Disable             0   0   0  */
constexpr uint8_t OPCODE_RDID                 	 = 0xAB;    /* Read Identification       0   0  >=1 */
constexpr uint8_t OPCODE_RDID_ALT             	 = 0x90;    /* Read Identification (alt) 0   0  >=1 */
constexpr uint8_t OPCODE_JEDEC_ID             	 = 0x9F;    /* JEDEC ID read             0   0  >=3 */
constexpr uint8_t OPCODE_EHLD                 	 = 0xAA;
constexpr uint8_t OPCODE_READ_SID                = 0x88;
constexpr uint8_t OPCODE_PROGRAM_SID             = 0xA5;
constexpr uint8_t OPCODE_LOCKOUT_SID             = 0x85;

constexpr uint8_t SST25_MANUFACTURER            = 0xBF;    /* SST manufacturer ID */
constexpr uint8_t SST25_DEVICE_ID               = 0x4B;    /* SSTVF032B device ID */

constexpr uint8_t SST25_JEDEC_MANUFACTURER    	= 0xBF;    /* SST manufacturer ID */
constexpr uint8_t SST25_JEDEC_MEMORY_TYPE     	= 0x25;    /* SST25 memory type */
constexpr uint8_t SST25_JEDEC_MEMORY_CAPACITY 	= 0x4B;    /* SST25VF032B memory capacity */

constexpr uint8_t SST25_SR_BUSY                 = (0x1 << 0);  /* Bit 0: Write in progress */
constexpr uint8_t SST25_SR_WEL                	= (0x1 << 1);  /* Bit 1: Write enable latch bit */
constexpr uint8_t SST25_SR_BP_MASK            	= (0xF << 2);
constexpr uint8_t SST25_SR_BP_NONE            	= (0x0 << 2);/* Unprotected */
constexpr uint8_t SST25_SR_BP_UPPER128        	= (0x1 << 2); /* Upper 64th */
constexpr uint8_t SST25_SR_BP_UPPER64         	= (0x2 << 2); /* Upper 32nd */
constexpr uint8_t SST25_SR_BP_UPPER32         	= (0x3 << 2); /* Upper 16th */
constexpr uint8_t SST25_SR_BP_UPPER16         	= (0x4 << 2); /* Upper 8th */
constexpr uint8_t SST25_SR_BP_UPPER8          	= (0x5 << 2); /* Upper quarter */
constexpr uint8_t SST25_SR_BP_UPPER4          	= (0x6 << 2); /* Upper half */
constexpr uint8_t SST25_SR_BP_UPPER2          	= (0x7 << 2); /* All sectors */
constexpr uint8_t SST25_SR_BP_ALL             	= (0x8 << 2); /* All sectors */
constexpr uint8_t SST25_SR_SEC                	= (0x1 << 6);  /* Bit 6: Auto Address increment programming */
constexpr uint8_t SST25_SR_BPL                	= (0x1 << 7);  /* Bit 7: Status register write protect */

constexpr uint8_t SST25_ERASED_STATE            = 0xFF;        /* State of FLASH when erased */
constexpr uint8_t DUMMY_BYTE                    = 0xFE;

constexpr uint16_t DISABLE_WRITE_PROTECTION     = 0x00;
constexpr uint16_t SECTOR_SIZE                  = 4096;
constexpr uint32_t MEMORY_SIZE                  = 0x800000;
constexpr uint16_t SECTOR_COUNT                 = MEMORY_SIZE / SECTOR_SIZE;
constexpr uint16_t PAGE_SIZE                    = 256;
constexpr uint16_t PAGE_COUNT_IN_SECTOR         = 16;


class SST25
{
public:
    SST25(SPI_ext *spi, Pinout nss_pin);
	int read_sectors(uint32_t sector, uint32_t count, void *buf);
	int write_sectors(uint32_t sector,  uint32_t count, const void *buf);
	void disable_wtite_protection();

	void read(uint32_t address, uint32_t count, uint8_t* buf);
	void read_high_speed(uint32_t address, uint32_t count, uint8_t* buf);
	void page_program(uint32_t address, uint8_t *data, uint16_t count);
	void erase_sector(uint32_t sector);
	void erase_block_32K(uint32_t start_sector);
	void erase_block_64K(uint32_t start_sector);
	void erase_full_chip();
	///TODO: 3 SID functions
	uint8_t read_status_register();
	void enable_write();
	void disable_write();
	void enable_write_status_register();
	void write_status_register(uint8_t data);
	///TODO: enable HOLD
	uint16_t read_id();
	uint32_t read_jedec_id();

private:
    SPI_ext *_spi;
    GPIO_ext *_nss;

    void select_device();
    void release_device();
    void write_buffer(const uint8_t *buf, int len);
    void read_buffer(uint8_t *buf, int len);
	void wait_write_complete();
};

#endif
