#ifndef RAMDISK_H
#define RAMDISK_H

#include <stdint.h>
#include <libopencm3/cm3/common.h>

BEGIN_DECLS
extern int ramdisk_init(void);
extern int ramdisk_read(uint32_t lba, uint8_t *copy_to);
extern int ramdisk_write(uint32_t lba, const uint8_t *copy_from);
extern int ramdisk_blocks(void);
END_DECLS

#endif
