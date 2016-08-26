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

#include <fs/drv/SST25.h>
#include "stdio.h"

static volatile bool data_received = false;
static volatile bool data_transmitted = false;

SST25::SST25()
: _spi(SPI_CONF), _cs(SPI_CS)
{
	_spi_config();
	_dma_config();
	_cs_config();
}

void SST25::disable_write_protection()
{
	enable_write_status_register();
	write_status_register(DISABLE_WRITE_PROTECTION);
}

void SST25::read(uint32_t address, uint32_t count, uint8_t* buf)
{
	_wait_write_complete();
	_select_device();
	_send_command(OPCODE_READ, address);
	_read_data(buf, count);
	_release_device();
}

void SST25::read_high_speed(uint32_t address, uint32_t count, uint8_t* buf)
{
	_wait_write_complete();
	_select_device();
	_send_command(OPCODE_FAST_READ, address, true);
	_read_data(buf, count);
	_release_device();
}

void SST25::page_program(uint32_t address, uint8_t *data, uint16_t count)
{
	_wait_write_complete();
	enable_write();
	_select_device();
	_send_command(OPCODE_PAGE_PROGRAM, address);
	_write_data(data, count);
	_release_device();
}

void SST25::erase_sector(uint32_t sector)
{
	uint32_t address = (sector * SECTOR_SIZE);

	_wait_write_complete();
	enable_write();
	_select_device();
	_send_command(OPCODE_SECTOR_ERASE, address);
	_release_device();
}

void SST25::erase_block_32K(uint32_t start_sector)
{
	uint32_t address = (start_sector * SECTOR_SIZE);

	_wait_write_complete();
	enable_write();
	_select_device();
	_send_command(OPCODE_BLOCK_ERASE_32K, address);
	_release_device();
}

void SST25::erase_block_64K(uint32_t start_sector)
{
	uint32_t address = (start_sector * SECTOR_SIZE);

	_wait_write_complete();
	enable_write();
	_select_device();
	_send_command(OPCODE_BLOCK_ERASE_64K, address);
	_release_device();
}

void SST25::erase_full_chip()
{
	_wait_write_complete();
	enable_write();
	_select_device();
	_send_command(OPCODE_CHIP_ERASE);
	_release_device();
}

uint8_t SST25::read_status_register()
{
	_select_device();
	_send_command(OPCODE_RDSR);
	uint8_t result;
	_read_data(&result, sizeof(uint8_t));
	_release_device();
	return (result);
}

void SST25::enable_write()
{
	_wait_write_complete();
	_select_device();
	_send_command(OPCODE_WREN);
	_release_device();
}

void SST25::disable_write()
{
	_wait_write_complete();
	_select_device();
	_send_command(OPCODE_WRDI);
	_release_device();
}

void SST25::enable_write_status_register()
{
	_select_device();
	_send_command(OPCODE_EWSR);
	_release_device();
}

void SST25::write_status_register(uint8_t data)
{
	_select_device();
	_send_command(OPCODE_WRSR);
	_write_data(&data, sizeof(uint8_t));
	_release_device();
}

uint16_t SST25::read_id()
{
	_select_device();
	_send_command(OPCODE_RDID_ALT, 0);
	uint16_t result = 0;
	_read_data((uint8_t*)&result, sizeof(uint16_t));
	_release_device();
	return (result);
}

uint32_t SST25::read_jedec_id()
{
	_select_device();
	_send_command(OPCODE_JEDEC_ID);
	uint32_t result;
	_read_data((uint8_t*)&result, 3);
	_release_device();
	return (result);
}

int SST25::read_sector(uint32_t sector, void *buf)
{
	if(sector >= (SECTOR_COUNT)) {
		return (0);
	}

	uint32_t address = (sector * SECTOR_SIZE);
	read_high_speed(address, FAKE_SECTOR_SIZE, (uint8_t*)buf);

	return (1);
}

int SST25::write_sector(uint32_t sector, const void *buf)
{
	if(sector >= (SECTOR_COUNT)) {
		return (0);
	}

	erase_sector(sector);
	uint32_t address = (sector * SECTOR_SIZE);

	for (int j = 0; j < PAGE_COUNT_IN_FAKE_SECTOR; j++) {
		page_program(address, (uint8_t*)buf, PAGE_SIZE);
		address += PAGE_SIZE;
		buf += PAGE_SIZE;
	}

	return (1);
}

void SST25::_spi_config()
{
	_spi.reset();
	_spi.disable();
	_spi.set_master_mode();
	_spi.set_baudrate_prescaler(BAUDRATE_FPCLK_DIV_2);
	_spi.set_standard_mode(MODE_0);
	_spi.set_data_drame_format(DFF_8BIT);
	_spi.set_bit_position(MSB_FIRST);
	_spi.enable_ss_output();
	_spi.set_software_slave_management(State::ENABLE);
	_spi.set_nss(HIGH);
	_spi.enable();
}

void SST25::_dma_config()
{
	rcc_periph_reset_pulse(rcc_periph_rst::RST_DMA2);

	dma_stream_reset(SPI_DMA, SPI_DMA_RX_STREAM);
	dma_set_peripheral_address(SPI_DMA, SPI_DMA_RX_STREAM, (uint32_t)&SPI_DR(SPI));
	dma_set_peripheral_size(SPI_DMA, SPI_DMA_RX_STREAM, DMA_SxCR_PSIZE_8BIT);
	dma_set_memory_size(SPI_DMA, SPI_DMA_RX_STREAM, DMA_SxCR_MSIZE_8BIT);
	dma_disable_peripheral_increment_mode(SPI_DMA, SPI_DMA_RX_STREAM);
	dma_set_transfer_mode(SPI_DMA, SPI_DMA_RX_STREAM, DMA_SxCR_DIR_PERIPHERAL_TO_MEM);
	dma_set_priority(SPI_DMA, SPI_DMA_RX_STREAM, DMA_SxCR_PL_HIGH);
	dma_channel_select(SPI_DMA, SPI_DMA_RX_STREAM, SPI_DMA_CHANNEL);
	dma_enable_transfer_complete_interrupt(SPI_DMA, SPI_DMA_RX_STREAM);
	nvic_set_priority(SPI_DMA_RX_NVIC, 0);
	nvic_enable_irq(SPI_DMA_RX_NVIC);

	dma_stream_reset(SPI_DMA, SPI_DMA_TX_STREAM);
	dma_set_peripheral_address(SPI_DMA, SPI_DMA_TX_STREAM, (uint32_t)&SPI_DR(SPI));
	dma_set_peripheral_size(SPI_DMA, SPI_DMA_TX_STREAM, DMA_SxCR_PSIZE_8BIT);
	dma_set_memory_size(SPI_DMA, SPI_DMA_TX_STREAM, DMA_SxCR_MSIZE_8BIT);
	dma_disable_peripheral_increment_mode(SPI_DMA, SPI_DMA_TX_STREAM);
	dma_set_transfer_mode(SPI_DMA, SPI_DMA_TX_STREAM, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
	dma_set_priority(SPI_DMA, SPI_DMA_TX_STREAM, DMA_SxCR_PL_HIGH);
	dma_channel_select(SPI_DMA, SPI_DMA_TX_STREAM, SPI_DMA_CHANNEL);
	dma_enable_transfer_complete_interrupt(SPI_DMA, SPI_DMA_TX_STREAM);
	nvic_set_priority(SPI_DMA_TX_NVIC, 0);
	nvic_enable_irq(SPI_DMA_TX_NVIC);
}

void SST25::_cs_config()
{
	_cs.mode_setup(GPIO_CPP_Extension::Mode::OUTPUT, GPIO_CPP_Extension::PullMode::NO_PULL);
	_cs.set_output_options(GPIO_CPP_Extension::OutputType::PUSH_PULL, GPIO_CPP_Extension::Speed::FAST_50MHz);
	_cs.set();
}

void SST25::_select_device()
{
	_cs.clear();
	while(_spi.get_flag_status(Flag::BUSY_FLAG));
}

void SST25::_release_device()
{
	_cs.set();
}

void SST25::_wait_write_complete()
{
	while (read_status_register() & SST25_SR_BUSY);
}

void SST25::_send_command(uint8_t command, int32_t address, bool dummy)
{
	uint8_t cmd[COMMAND_SIZE];
	uint32_t length;

	cmd[0] = command;
	length = 1;

	if (address >= 0) {
		cmd[1] = (uint8_t)((address >> 0x10) & 0xFF);
		cmd[2] = (uint8_t)((address >> 0x08) & 0xFF);
		cmd[3] = (uint8_t)((address >> 0x00) & 0xFF);
		length = 4;
	}

	if (dummy) {
		cmd[4] = DUMMY_BYTE;
		length = 5;
	}

	_transfer_data(false, cmd, length);
}

void SST25::_transfer_data(bool receive, uint8_t *buf, uint16_t len)
{
	uint16_t rw_workbyte = 0xffff;

	if (len <= COMMAND_SIZE) {
		if (receive) {
			while(len--) {
				*buf++ = _spi.read(0x00);
			}
		}

		else {
			while(len--) {
				_spi.write(*buf++);
			}
			_spi.write_end();
		}
	}

	else {
		if (receive) {
			dma_set_memory_address(SPI_DMA, SPI_DMA_RX_STREAM, (uint32_t)buf);
			dma_set_number_of_data(SPI_DMA, SPI_DMA_RX_STREAM, len);
			dma_enable_memory_increment_mode(SPI_DMA,SPI_DMA_RX_STREAM);

			dma_set_memory_address(SPI_DMA, SPI_DMA_TX_STREAM, (uint32_t)&rw_workbyte);
			dma_set_number_of_data(SPI_DMA, SPI_DMA_TX_STREAM, len);
			dma_disable_memory_increment_mode(SPI_DMA,SPI_DMA_TX_STREAM);
		}

		else {
			dma_set_memory_address(SPI_DMA, SPI_DMA_RX_STREAM, (uint32_t)&rw_workbyte);
			dma_set_number_of_data(SPI_DMA, SPI_DMA_RX_STREAM, len);
			dma_disable_memory_increment_mode(SPI_DMA,SPI_DMA_RX_STREAM);

			dma_set_memory_address(SPI_DMA, SPI_DMA_TX_STREAM, (uint32_t)buf);
			dma_set_number_of_data(SPI_DMA, SPI_DMA_TX_STREAM, len);
			dma_enable_memory_increment_mode(SPI_DMA,SPI_DMA_TX_STREAM);
		}

		dma_enable_stream(SPI_DMA, SPI_DMA_RX_STREAM);
		dma_enable_stream(SPI_DMA, SPI_DMA_TX_STREAM);

		_spi.enable_rx_dma();
		_spi.enable_tx_dma();

		while (!data_received && !data_transmitted);
		data_received = false;
		data_transmitted = false;

		dma_disable_stream(SPI_DMA, SPI_DMA_RX_STREAM);
		dma_disable_stream(SPI_DMA, SPI_DMA_TX_STREAM);

		_spi.disable_rx_dma();
		_spi.disable_tx_dma();
	}
}

void SPI_DMA_RX_IRQ(void)
{
	if (dma_get_interrupt_flag(SPI_DMA, SPI_DMA_RX_STREAM, DMA_TCIF)) {
		dma_clear_interrupt_flags(SPI_DMA, SPI_DMA_RX_STREAM, DMA_TCIF);
		data_received = true;
	}
}

void SPI_DMA_TX_IRQ(void)
{
	if (dma_get_interrupt_flag(SPI_DMA, SPI_DMA_TX_STREAM, DMA_TCIF)) {
		dma_clear_interrupt_flags(SPI_DMA, SPI_DMA_TX_STREAM, DMA_TCIF);
		data_transmitted = true;
	}
}
