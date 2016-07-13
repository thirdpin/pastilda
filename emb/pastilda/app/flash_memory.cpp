#include "flash_memory.h"
#include "systick_ext.h"

#define WBVAL(x) ((x) & 0xFF), (((x) >> 8) & 0xFF)
#define QBVAL(x) ((x) & 0xFF), (((x) >> 8) & 0xFF), (((x) >> 16) & 0xFF), (((x) >> 24) & 0xFF)
#define BYTES_PER_SECTOR	512
#define SECTORS_PER_CLUSTER	4
#define RESERVED_SECTORS	1
#define FAT_COPIES		    2
#define ROOT_ENTRIES		512

FlashMemory *flash_pointer;

FlashMemory::FlashMemory(struct block_device *dev)
{
	flash_pointer = this;

	SPI_CPP_Extension::SPI_Conf sst25_conf = { 1, PB5, PB4, PB3 };
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

	_sst25 = new SST25(_spi, PA15);
	_sst25->disable_wtite_protection();

	dev->get_sector_size = get_sector_size;
	dev->read_sectors = read_sectors;
	dev->write_sectors = write_sectors;
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

	uint32_t real_start_sector = sector / 8;
	uint32_t real_end_sector = end_sector / 8;
	uint32_t real_sector_count = real_end_sector - real_start_sector + 1;

	uint32_t data_size = real_sector_count * SECTOR_SIZE;
    uint8_t data[data_size];

	uint32_t ok_sectors = flash_pointer->_sst25->read_sectors(real_start_sector, real_sector_count, data);

	if (ok_sectors != real_sector_count) {
		return (0);
	}

	uint32_t start_address = (real_start_sector * SECTOR_SIZE) + ((sector % 8) * FAKE_SECTOR_SIZE);
	uint32_t end_address = start_address + (count * FAKE_SECTOR_SIZE);

	for (int i = start_address, j = 0; i < end_address; i++, j++) {
		((uint8_t*)buf)[j] = data[i];
	}

	return (count);
}

int FlashMemory::write_sectors(const struct block_device *dev, uint32_t sector, uint32_t count, const void *buf)
{
	(void)dev;
	uint32_t end_sector = sector + count - 1;

	uint32_t real_start_sector = sector / 8;
	uint32_t real_end_sector = end_sector / 8;
	uint32_t real_sector_count = real_end_sector - real_start_sector + 1;


	uint32_t copy_size = real_sector_count * SECTOR_SIZE;
	uint8_t sector_copy[copy_size];

	uint32_t ok_sectors = flash_pointer->_sst25->read_sectors(real_start_sector, real_sector_count, sector_copy);

	if (ok_sectors != real_sector_count) {
		return (0);
	}

	for (int i = 0; i < real_sector_count; i++) {
		flash_pointer->_sst25->erase_sector(real_start_sector + i);
	}

	uint32_t start_address = (real_start_sector * SECTOR_SIZE) + ((sector % 8) * FAKE_SECTOR_SIZE);
	uint32_t end_address = start_address + (count * FAKE_SECTOR_SIZE);

	for (int i = start_address, j = 0; i <= end_address; i++, j++) {
		sector_copy[i] = ((uint8_t*)buf)[j];
	}

	ok_sectors = flash_pointer->_sst25->write_sectors(real_start_sector, real_sector_count, sector_copy);
	if (ok_sectors != real_sector_count) {
		return (0);
	}


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
int FlashMemory::flash_write(uint32_t lba, const uint8_t *copy_from)
{
	///TODO: check for lba > 3
	if (lba >= FAKE_SECTOR_COUNT) {
		return (1);
	}
	else {
		write_sectors(0, lba, 1, copy_from);
		return (1);
	}
	// ignore writes
	return (0);
}
int FlashMemory::flash_blocks(void)
{
	return (FAKE_SECTOR_COUNT);
}
