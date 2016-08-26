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

#ifndef KEEPASS_READER_H
#define KEEPASS_READER_H

#include <fs/file_system.h>
#include <stdint.h>
#include <string.h>
#include "keepass_crypto.h"
#include "keepass_reader_defines.h"
extern "C" {
#include "mxml.h"
}

namespace KeepAss
{
	class KeePassReader
	{
	public:
		KeePassReader();
		void set_password(const char* pass, uint32_t len);
		DecryptionResult decrypt_database(const char *db_name);
		mxml_node_t *get_xml();

	private:
		static constexpr uint8_t IV_SALSA[8] = {0xE8, 0x30, 0x09, 0x4B, 0x97, 0x20, 0x5D, 0x2A};
		FIL _file;
		uint32_t _signature1;
		uint32_t _signature2;
		uint32_t _signature3;
		HeaderField _header[HEADER_FIELD_COUNT];
		uint8_t _pass[MAX_PASSWORD_SIZE_IN_BYTES];
		uint32_t _pass_len;
		uint8_t _keyf[MAX_KEYFILE_SIZE_IN_BYTES];
		uint32_t _keyf_len;
		uint8_t _master_key[HASH_LENGTH];
		KeePassCredentials _credentials;
		uint8_t _decrypted_data[MAX_DATABASE_SIZE_IN_BYTES];
		uint32_t _file_len;
		mxml_node_t *_tree;

		DecryptionResult _checkKeePassVersion();
		void _readHeader();
		void _makeMasterKey(uint8_t *key, uint32_t key_len);
		void _makeMasterKey(uint8_t *pass, uint8_t *keyfile, uint32_t pass_len, uint32_t keylile_len);
		void _makeKeyRoutine(uint8_t *key_hash);
		DecryptionResult _decrypt();
		DecryptionResult _process_blocks();
		void _decrypt_passwords();

	};
}
#endif
