#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <string.h>
#include "blockdev.h"

#include "spi_ext.h"
#include "SST25.h"

using namespace SPI_CPP_Extension;

constexpr uint16_t FAKE_SECTOR_SIZE				= 512;
constexpr uint16_t FAKE_SECTOR_COUNT            = (MEMORY_SIZE / FAKE_SECTOR_SIZE);

class FlashMemory
{
public:
	FlashMemory(struct block_device *dev);
	static uint16_t get_sector_size(const struct block_device *dev);
	static int read_sectors(const struct block_device *dev, uint32_t sector, uint32_t count, void *buf);
	static int write_sectors(const struct block_device *dev, uint32_t sector, uint32_t count, const void *buf);

	int flash_read(uint32_t lba, uint8_t *copy_to);
	int flash_write(uint32_t lba, const uint8_t *copy_from);
	int flash_blocks(void);
private:
	SST25 *_sst25;
	SPI_ext *_spi;
};

#endif
