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

#ifndef SST25_H
#define SST25_H

#include <string.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
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

constexpr uint8_t SST25_MANUFACTURER            = 0xBF;
constexpr uint8_t SST25_DEVICE_ID               = 0x4B;

constexpr uint8_t SST25_JEDEC_MANUFACTURER    	= 0xBF;
constexpr uint8_t SST25_JEDEC_MEMORY_TYPE     	= 0x25;
constexpr uint8_t SST25_JEDEC_MEMORY_CAPACITY 	= 0x4B;

constexpr uint8_t SST25_SR_BUSY                 = (0x1 << 0); /* Bit 0: Write in progress */
constexpr uint8_t SST25_SR_WEL                	= (0x1 << 1); /* Bit 1: Write enable latch bit */
constexpr uint8_t SST25_SR_BP_MASK            	= (0xF << 2);
constexpr uint8_t SST25_SR_BP_NONE            	= (0x0 << 2); /* Unprotected */
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
constexpr uint16_t FAKE_SECTOR_SIZE				= 512;
constexpr uint32_t MEMORY_SIZE                  = 0x800000;
constexpr uint16_t SECTOR_COUNT                 = MEMORY_SIZE / SECTOR_SIZE;
constexpr uint16_t PAGE_SIZE                    = 256;
constexpr uint16_t PAGE_COUNT_IN_SECTOR         = 16;
constexpr uint16_t PAGE_COUNT_IN_FAKE_SECTOR    = 2;
constexpr uint16_t COMMAND_SIZE          		= 5;

constexpr Pinout   SPI_CS						= PA4;
constexpr uint32_t SPI                          = SPI1;
constexpr SPI_Conf SPI_CONF                     = {1, PA7, PA6, PA5};
constexpr uint32_t SPI_DMA                      = DMA2;
constexpr uint32_t SPI_DMA_RX_STREAM            = DMA_STREAM0;
constexpr uint32_t SPI_DMA_TX_STREAM            = DMA_STREAM3;
constexpr uint32_t SPI_DMA_CHANNEL              = DMA_SxCR_CHSEL_3;
constexpr uint32_t SPI_DMA_RX_NVIC              = NVIC_DMA2_STREAM0_IRQ;
constexpr uint32_t SPI_DMA_TX_NVIC              = NVIC_DMA2_STREAM3_IRQ;


#define SPI_DMA_RX_IRQ                          dma2_stream0_isr
#define SPI_DMA_TX_IRQ                          dma2_stream3_isr

extern "C" void SPI_DMA_RX_IRQ();
extern "C" void SPI_DMA_TX_IRQ();

class SST25
{
public:
	SPI_ext _spi;

    SST25();
	void disable_write_protection();
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

	int read_sector(uint32_t sector, void *buf);
	int write_sector(uint32_t sector, const void *buf);

private:

    GPIO_ext _cs;

    void _spi_config();
    void _dma_config();
    void _cs_config();
    void _select_device();
    void _release_device();
	void _send_command(uint8_t command, int32_t address = -1, bool dummy = false);
	void _wait_write_complete();
	void _transfer_data(bool receive, uint8_t *buf, uint16_t len);

	void _read_data(uint8_t *buf, int len)
	{
		_transfer_data(true, buf, len);
	}

	void _write_data(uint8_t *buf, int len)
	{
		_transfer_data(false, buf, len);
	}
};

#endif
