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

/* FAT Directory .
 */

#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "openfat.h"

#include "openfat/leaccess.h"
#include "openfat/blockdev.h"
#include "fat_core.h"
#include "direntry.h"

#define LONG_NAME_SUPPORT

#ifdef LONG_NAME_SUPPORT
uint8_t _fat_dirent_chksum(uint8_t *dosname)
{
	uint8_t sum = 0;
	int i;

	for (i = 0; i < 11; i++) 
		sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *dosname++;

	return sum;
}

/* Used to convert W95 UTF-16 filenames to ascii.
 * 0 means terminating null reached.
 * 1 means converted to end on input.
 * 2 means conversion error.
 */
static int ascii_from_utf16(char *ascii, const uint16_t *utf16, int count)
{
	uint16_t tmp;
	while(count--) {
		tmp = __get_le16(utf16++);
		if(tmp > 127) 
			return 2;
		*ascii++ = tmp;
		if(tmp == 0)
			return 0;
	}
	return 1;
}
#endif

int fat_readdir(struct fat_file_handle *h, struct dirent *ent)
{
#ifdef LONG_NAME_SUPPORT
	uint16_t csum = -1;
#endif
	struct fat_sdirent fatent;
	int i, j;

	while(fat_read(h, &fatent, sizeof(fatent)) == sizeof(fatent)) {

		if(fatent.name[0] == 0) 
			return -1;	/* Empty entry, end of directory */
		if(fatent.name[0] == (char)0xe5)
			continue;	/* Deleted entry */
		if(fatent.attr == FAT_ATTR_VOLUME_ID)
			continue;	/* Ignore volume id entry */
		if(fatent.attr == FAT_ATTR_LONG_NAME) {
#ifdef LONG_NAME_SUPPORT
			struct fat_ldirent *ld = (void*)&fatent;
			if(ld->ord & FAT_LAST_LONG_ENTRY) {
				memset(ent->d_name, 0, sizeof(ent->d_name));
				csum = ld->checksum;
			} 
			if(csum != ld->checksum) /* Abandon orphaned entry */
				csum = -1;

			i = ((ld->ord & 0x3f) - 1) * 13;

			/* If entries can't be converted to ASCII, abandon
			 * the long filename.  DOS 8.3 name will be returned.
			 * Not pretty... */
			switch(ascii_from_utf16(&ent->d_name[i], ld->name1, 5))
				{ case 0: continue; case 2: csum = -1; }
			switch(ascii_from_utf16(&ent->d_name[i+5], ld->name2, 6))
				{ case 0: continue; case 2: csum = -1; }
			switch(ascii_from_utf16(&ent->d_name[i+11], ld->name3, 2))
				{ case 0: continue; case 2: csum = -1; }
#endif
			continue;
		}
#ifdef LONG_NAME_SUPPORT
		if(csum != _fat_dirent_chksum((uint8_t*)fatent.name)) 
			ent->d_name[0] = 0;

		if(ent->d_name[0] == 0) {
#endif
			for(i = 0, j = 0; i < 11; i++, j++) {
				ent->d_name[j] = tolower(fatent.name[i]);
				if(fatent.name[i] == ' ') {
					ent->d_name[j] = '.';
					while((fatent.name[++i] == ' ') && (i < 11));
				}
			} 
			if(ent->d_name[j-1] == '.')
				ent->d_name[j-1] = 0;

			ent->d_name[j] = 0;
#ifdef LONG_NAME_SUPPORT
		}
#endif
		/* Non-standard */
		ent->fat_attr = fatent.attr;
		memcpy(ent->fat_sname, fatent.name, 11);

		return 0;
	}
	return -1;
}

/* Seek to a place in the directory suitable for writing 'entries' new
 * directory entries.  Called when creating files. */
int _fat_dir_seek_empty(struct fat_file_handle *dir, int entries)
{
	uint32_t pos = 0;
	struct fat_sdirent ent;
	int i = 0;

	fat_lseek(dir, 0, SEEK_SET);

	while(fat_read(dir, &ent, sizeof(ent)) == sizeof(ent)) {
		if(ent.name[0] == 0) /* Empty entry, end of directory */
			break;
		if(ent.name[0] == (char)0xe5) {	/* Deleted entry */
			i++;
			if(i == entries) 
				break;
			continue;	
		}
		i = 0;
		pos = dir->position;
	}

	fat_lseek(dir, pos, SEEK_SET);
	return 0;
}

static int fat_comparesfn(const char * name, const char *fatname)
{
	char canonname[11];
	int i;

	memset(canonname, ' ', sizeof(canonname));
	if(name[0] == '.') {
		/* Special case:
		 * Only legal names are '.' and '..' */
		memcpy(canonname, name, strlen(name));
		name += strlen(name);
	} else for(i = 0; (i < 11) && *name && (*name != '/'); i++) {
		if(*name == '.') {
			if(i < 8) continue;
			if(i == 8) name++;
		}
		canonname[i] = toupper(*name++);
	}
	return ((*name == 0) || (*name == '/')) && !memcmp(canonname, fatname, 11);
}

int fat_open(struct fat_vol_handle *vol, const char *name, int flags,
		struct fat_file_handle *file)
{
	struct fat_file_handle *dir = (struct fat_file_handle*)&vol->cwd;
	struct dirent dirent;

	/* FIXME: Implement flags O_RDONLY, O_WRONLY, O_RDWR. */

	if(strcmp(name, ".") == 0) {
		/* Special case needed for root dir with no '.' entry */
		memcpy(file, &vol->cwd, sizeof(*file));
		return 0;
	}

	fat_lseek(dir, 0, SEEK_SET);
	while(fat_readdir(dir, &dirent) == 0) {

		/* Check for name match */
		if((strcmp(name, dirent.d_name) == 0) ||
		   fat_comparesfn(name, dirent.fat_sname)) {
			/* reread on-disk directory entry */
			struct fat_sdirent fatent;
			uint32_t sector;
			uint16_t offset;
			/* Rewind directory one entry */
			fat_lseek(dir, -sizeof(fatent), SEEK_CUR);
			_fat_file_sector_offset(dir, &sector, &offset);
			if(fat_read(dir, &fatent, sizeof(fatent)) != 32)
				return -EIO;

			_fat_file_init(dir->fat, &fatent, file);
			file->flags = flags;
			if(!(fatent.attr & FAT_ATTR_DIRECTORY)) {
				file->dirent_sector = sector;
				file->dirent_offset = offset;
			} else if(!file->first_cluster) {
				/* Check for special case of root dir */
				_fat_file_root(dir->fat, file);
			}
			return 0;
		}
	}
	return -ENOENT;
}

int fat_chdir(struct fat_vol_handle *vol, const char *name)
{
	return fat_open(vol, name, 0, &vol->cwd);
}

