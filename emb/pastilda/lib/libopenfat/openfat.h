/*
 * This file is part of the openfat project.
 *
 * Copyright (C) 2011  Department of Physics, University of Otago
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
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

/** \file openfat.h 
 * \brief FAT Filesystem implementation, public interface.
 */

#ifndef __OPENFAT_H
#define __OPENFAT_H

#include <stdint.h>

#include <sys/stat.h>

#include "openfat/blockdev.h"

/* Forward declarations of private structures. */

/** \brief Structure used internally for FAT volume state.
 * Do not access directly.  Structure has no public fields. 
 */
typedef struct fat_vol_handle FatVol;

/** \brief Structure used internally for FAT file state.
 * Do not access directly.  Structure has no public fields. 
 */
typedef struct fat_file_handle FatFile;

/** \brief Mount a FAT volume. 
 *
 * Initialise a handle for access to a FAT filesystem on the specified
 * block device.
 * \param dev Pointer to block device to mount.
 * \param vol Pointer to filesystem handle to initialise.
 * \return 0 on success.
 */
int __attribute__((warn_unused_result))
fat_vol_init(const struct block_device *dev, FatVol *vol);

/** \brief Change current working directory. 
 * \param vol Pointer to FAT volume handle.
 * \param name Directory name to change to.  Relative to the current dir.
 * \return 0 on success.
 */
int fat_chdir(FatVol *vol, const char *name);
/** \brief Create a new directory.
 * \param vol Pointer to FAT volume handle.
 * \param name Directory name to create.
 * \return 0 on success.
 */
int fat_mkdir(FatVol *vol, const char *name);
/** \brief Remove an empty directory.
 * \param vol Pointer to FAT volume handle.
 * \param name Directory name to remove.
 * \return 0 on success.
 */
int fat_rmdir(FatVol *vol, const char *name); /* TODO */

/** \brief Open an existing file.
 * The option O_ASYNC may be passed in flags to surpress the automatic
 * updating of the files directory entry on writes.  fat_file_sync() must be
 * called explicitly to update it in this case.
 *
 * This function uses a ::dirent on the stack for iterating over the directory.
 * This is a fairly large structure.
 *
 * \param vol Pointer to FAT volume handle.
 * \param name File name in current directory to open.
 * \param flags O_RDONLY, O_WRONLY, or O_RDWR. (currently not implemented)
 * \param file Pointer to file handle to initialise.
 * \return 0 on success.
 */
int __attribute__((warn_unused_result))
fat_open(FatVol *vol, const char *name, int flags, FatFile *file);

/** \brief Create a new file.
 * \see fat_open()
 *
 * \param vol Pointer to FAT volume handle.
 * \param name File name in current directory to create.
 * \param flags O_RDONLY, O_WRONLY, or O_RDWR. (currently not implemented)
 * \param file Pointer to file handle to initialise.
 * \return 0 on success.
 */
int __attribute__((warn_unused_result))
fat_create(FatVol *vol, const char *name, int flags, FatFile *file);

#define O_ASYNC 020000
/** \brief Update an open file's directory entry.
 * This must be called explicitly if a file is opened with the flag O_ASYNC.
 * In this case, updates to the directory entry are surpressed on writes to
 * improve performance.
 * \param file Pointer to file handle from which to read.
 * \return 0 on success.
 */
int fat_file_sync(FatFile *h);

/** \brief Read from an open file.
 * \param file Pointer to file handle from which to read.
 * \param buf Buffer into which to read.
 * \param size Number of bytes to read.
 * \return Number of bytes read on success, negative on error.
 */
int __attribute__((warn_unused_result))
fat_read(FatFile *file, void *buf, int size);

/** \brief Write to an open file.
 * \param file Pointer to file handle into which to write.
 * \param buf Buffer from which to write.
 * \param size Number of bytes to write.
 * \return Number of bytes written on success, negative on error.
 */
int __attribute__((warn_unused_result))
fat_write(FatFile *file, const void *buf, int size);

/** \brief Sets the position in an open file.
 * \param file Pointer to file handle to change.
 * \param offset Offset into target file.
 * \param whence One of SEEK_SET, SEEK_CUR, SEEK_END.  See Unix documentation.
 * \return New file position on success, negative on error.
 */
off_t fat_lseek(FatFile *file, off_t offset, int whence);

/** \brief Unlink/delete a file.
 * \param vol Pointer to FAT volume handle.
 * \param name Name of file in current directory to unlink.
 * \return 0 on success.
 */
int fat_unlink(FatVol *vol, const char *name);

#define FAT_ATTR_READ_ONLY	0x01
#define FAT_ATTR_HIDDEN		0x02
#define FAT_ATTR_SYSTEM		0x04
#define FAT_ATTR_VOLUME_ID	0x08
#define FAT_ATTR_DIRECTORY	0x10
#define FAT_ATTR_ARCHIVE	0x20
#define FAT_ATTR_LONG_NAME	0x0F

/** \brief Public directory entry structure, returned by fat_readdir() */
struct dirent {
	char d_name[256];	/**< Long file name, POSIX standard. */
	/* Non-standard */
	uint8_t fat_attr;	/**< FAT file attributes, non-standard */
	char fat_sname[11];	/**< DOS short filename, non-standard */
};

/** \brief Read a directory entry.
 * The dirent structure is filled in with the details of the next file in
 * the directory stream pointed to by dir.
 * \param dir Pointer to file handle for the directory to be read.
 * \param ent Pointer to dirent structure for the read entry.
 * \return 0 on success.
 */
int fat_readdir(FatFile *dir, struct dirent *ent);


/* Everything below is private.  Applications should not direcly access
 * anything here.
 */

struct fat_file_handle {
	struct fat_vol_handle *fat;
	/* Fields from dir entry */
	uint32_t size;
	uint32_t first_cluster;
	/* Internal state information */
	uint32_t position;
	uint32_t cur_cluster;	/* This is used for sector on FAT12/16 root */
	uint8_t root_flag;	/* Flag to mark root directory on FAT12/16 */
	int flags;
	/* Reference to dirent */
	uint32_t dirent_sector;
	uint16_t dirent_offset;
};

struct fat_vol_handle {
	const struct block_device *dev;
	/* FAT type: 12, 16 or 32 */
	int type;
	/* Useful fields from BPB */
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	uint8_t num_fats;
	/* Fields calcuated from BPB */
	uint32_t first_data_sector;
	uint32_t cluster_count;
	uint32_t fat_size;
	union {
		struct {
			uint32_t root_cluster;
		} fat32;
		struct {
			uint16_t root_sector_count;
			uint16_t root_first_sector;
		} fat12_16;
	};
	/* Internal state */
	uint32_t last_cluster_alloc;
	struct fat_file_handle cwd;
};

#endif
