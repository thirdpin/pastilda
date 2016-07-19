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

#include "SST25.h"

SST25::SST25(SPI_ext *spi, Pinout nss_pin)
{
	_spi = spi;

	_nss = new GPIO_ext(nss_pin);
	_nss->mode_setup(GPIO_CPP_Extension::Mode::OUTPUT, GPIO_CPP_Extension::PullMode::NO_PULL);
	_nss->set_output_options(GPIO_CPP_Extension::OutputType::PUSH_PULL, GPIO_CPP_Extension::Speed::FAST_50MHz);
	_nss->set();
}

int SST25::read_sectors(uint32_t sector, uint32_t count, void *buf)
{
	if(sector >= (SECTOR_COUNT)) {
		return (0);
	}

	uint32_t address = (sector * SECTOR_SIZE);
	uint32_t data_len = SECTOR_SIZE * count;

	while ((address + data_len - 1) > (MEMORY_SIZE - 1)) {
		count -= 1;
		data_len = SECTOR_SIZE * count;
	}

	read(address, data_len, (uint8_t*)buf);
	return (count);
}
int SST25::write_sectors(uint32_t sector, uint32_t count, const void *buf)
{
	if(sector >= (SECTOR_COUNT)) {
		return (0);
	}

	uint32_t start_address = (sector * SECTOR_SIZE);
	uint32_t data_len = SECTOR_SIZE * count;

	for(int i = 0; i < count; i++)
	{
		erase_sector(sector + i);
		uint32_t address = ((sector + i) * SECTOR_SIZE);

		for (int j = 0; j < PAGE_COUNT_IN_SECTOR; j++) {
			page_program(address, (uint8_t*)buf, PAGE_SIZE);
			address += PAGE_SIZE;
			buf += PAGE_SIZE;
		}
	}
	return (count);
}
void SST25::disable_wtite_protection()
{
	enable_write_status_register();
	write_status_register(DISABLE_WRITE_PROTECTION);
}

void SST25::read(uint32_t address, uint32_t count, uint8_t* buf)
{
	wait_write_complete();
	select_device();
	uint8_t tx[4] = {OPCODE_READ, ((address >> 16) & 0xFF), ((address >> 8) & 0xFF), (address & 0xFF)};
	write_buffer(tx, 4);
	uint8_t rx[count];
	read_buffer(rx, count);
	release_device();
	memcpy(buf, rx, count);
	//return (rx);
}
void SST25::read_high_speed(uint32_t address, uint32_t count, uint8_t* buf)
{
	wait_write_complete();
	select_device();
	uint8_t tx[5] = {OPCODE_FAST_READ, ((address >> 16) & 0xFF), ((address >> 8) & 0xFF), (address & 0xFF), DUMMY_BYTE};
	write_buffer(tx, 5);
	uint8_t rx[count];
	read_buffer(rx, count);
	release_device();
	memcpy(buf, rx, count);
	//return (rx);
}
void SST25::page_program(uint32_t address, uint8_t *data, uint16_t count)
{
	wait_write_complete();
	enable_write();
	select_device();
	uint16_t total_size = (4 + count);
	uint8_t tx[total_size] = {OPCODE_PAGE_PROGRAM, ((address >> 16) & 0xFF), ((address >> 8) & 0xFF), (address & 0xFF)};
	for (int i = 0, j = 4; i < count; i++, j++) {
		tx[j] = data[i];
	}
	write_buffer(tx, total_size);
	release_device();
}
void SST25::erase_sector(uint32_t sector)
{
	wait_write_complete();
	enable_write();
	uint32_t address = (sector * SECTOR_SIZE);
	select_device();
	uint8_t tx[4] = {OPCODE_SECTOR_ERASE, ((address >> 16) & 0xFF), ((address >> 8) & 0xFF), (address & 0xFF)};
	write_buffer(tx, 4);
	release_device();
}
void SST25::erase_block_32K(uint32_t start_sector)
{
	wait_write_complete();
	enable_write();
	uint32_t address = (start_sector * SECTOR_SIZE);
	select_device();
	uint8_t tx[4] = {OPCODE_BLOCK_ERASE_32K, ((address >> 16) & 0xFF), ((address >> 8) & 0xFF), (address & 0xFF)};
	write_buffer(tx, 4);
	release_device();
}
void SST25::erase_block_64K(uint32_t start_sector)
{
	wait_write_complete();
	enable_write();
	uint32_t address = (start_sector * SECTOR_SIZE);
	select_device();
	uint8_t tx[4] = {OPCODE_BLOCK_ERASE_64K, ((address >> 16) & 0xFF), ((address >> 8) & 0xFF), (address & 0xFF)};
	write_buffer(tx, 4);
	release_device();
}
void SST25::erase_full_chip()
{
	wait_write_complete();
	enable_write();
	select_device();
	_spi->send(OPCODE_CHIP_ERASE);
	release_device();
}
uint8_t SST25::read_status_register()
{
	select_device();
	uint8_t tx[1] = {OPCODE_RDSR};
	write_buffer(tx, 1);
	uint8_t rx[1];
	read_buffer(rx, 1);
	release_device();
	return (rx[0]);
}
void SST25::enable_write()
{
	wait_write_complete();
	select_device();
	_spi->send(OPCODE_WREN);
	release_device();
}
void SST25::disable_write()
{
	wait_write_complete();
	select_device();
	_spi->send(OPCODE_WRDI);
	release_device();
}
void SST25::enable_write_status_register()
{
	//wait_write_complete();
	select_device();
	_spi->send(OPCODE_EWSR);
	release_device();
}
void SST25::write_status_register(uint8_t data)
{
	//wait_write_complete();
	select_device();
	uint8_t tx[2] = {OPCODE_WRSR, data};
	write_buffer(tx, 2);
	release_device();
}
uint16_t SST25::read_id()
{
	select_device();
	uint8_t tx[4] = {OPCODE_RDID_ALT, 0, 0, 0};
	write_buffer(tx, 4);
	uint8_t rx[2];
	read_buffer(rx, 2);
	release_device();
	return ((rx[0] << 8) | rx[1]);
}
uint32_t SST25::read_jedec_id()
{
	select_device();
	uint8_t tx[1] = {OPCODE_JEDEC_ID};
	write_buffer(tx, 1);
	uint8_t rx[3];
	read_buffer(rx, 3);
	release_device();
	return ((rx[0] << 16) | (rx[1] << 8) | rx[3]);
}

void SST25::select_device()
{
	_nss->clear();
	while(_spi->get_flag_status(Flag::BUSY_FLAG));
}
void SST25::release_device()
{
	_nss->set();
}
void SST25::write_buffer(const uint8_t *buf, int len)
{
	while(len--)
		_spi->send(*buf++);
}
void SST25::read_buffer(uint8_t *buf, int len)
{
	uint8_t res = _spi->read();
	while(len--)
		*buf++ = _spi->read();
}
void SST25::wait_write_complete()
{
	uint8_t status = read_status_register();
	while (status & SST25_SR_BUSY) {
		status = read_status_register();
	}
}
