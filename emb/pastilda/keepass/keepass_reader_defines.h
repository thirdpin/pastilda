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

#ifndef KEEPASS_READER_DEFINES_H
#define KEEPASS_READER_DEFINES_H

namespace KeepAss
{
	constexpr uint32_t SIGNATURE_1                        	= 0x9AA2D903;
	constexpr uint32_t SIGNATURE_2_2X_PRE_RELEASE 			= 0xB54BFB66;
	constexpr uint32_t SIGNATURE_2_2X_POST_RELEASE 			= 0xB54BFB67;
	constexpr uint32_t HEADER_OFFSET_IN_BYTES 				= 8;
	constexpr uint32_t MAX_PASSWORD_SIZE_IN_BYTES			= 32;
	constexpr uint32_t MAX_KEYFILE_SIZE_IN_BYTES			= 32;
	constexpr uint32_t HASH_LENGTH 							= 32;
	constexpr uint32_t COMPOSITE_KEY_LENGTH 				= 64;
	constexpr uint32_t MASTER_KEY_LENGTH_2X                 = 64;
	constexpr uint32_t MAX_DATABASE_SIZE_IN_BYTES 			= 30000;
	constexpr uint32_t MAX_HEADER_FIELD_SIZE                = 32;

	#pragma pack(push, 1)
	typedef struct
	{
		uint16_t size;
		uint8_t data[MAX_HEADER_FIELD_SIZE];
	} HeaderField;

	typedef struct
	{
		uint32_t blockID;
		uint8_t blockDataHash[HASH_LENGTH];
		uint32_t blockDataSize;
	} BlockDataHeader;
	#pragma pack(pop)


	typedef enum : uint8_t
	{
		END_OF_HEADER = 0,
		COMMENT = 1,
		CIPHER_ID = 2,
		COMPRESSION_FLAGS = 3,
		MASTER_SEED = 4,
		TRANSFORM_SEED = 5,
		TRANSFORM_ROUNDS = 6,
		ENCRYPTION_IV = 7,
		PROTECTED_STREAM_KEY = 8,
		STREAM_START_BYTES = 9,
		INNER_RANDOM_STREAM_ID = 10,
		HEADER_FIELD_COUNT = 11
	} HeaderFieldName;

	typedef enum
	{
		NOT_SELECTED,
		PASSWORD,
		KEY_FILE,
		PASSWORD_AND_KEY_FILE
	} KeePassCredentials;

	typedef enum
	{
		SUCCESS,
		SIGNATURE_ERROR,
		MASTER_KEY_ERROR,
		CREDENTIALS_ERROR,
		DB_FILE_ERROR,
		DATA_HASH_ERROR,
	} DecryptionResult;
}
#endif
