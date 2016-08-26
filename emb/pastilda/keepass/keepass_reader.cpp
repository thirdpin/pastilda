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

#include <keepass_reader.h>

using namespace KeepAss;

constexpr uint8_t KeePassReader::IV_SALSA[8];

KeePassReader::KeePassReader()
{
	_tree = nullptr;
	_signature1 = 0;
	_signature2 = 0;
	_signature3 = 0;
	_keyf_len = 0;
	_pass_len = 0;
	_file_len = 0;
	_credentials = KeePassCredentials::NOT_SELECTED;
}

DecryptionResult KeePassReader::_checkKeePassVersion()
{
	FileSystem::read_next_file_chunk(&_file, &_signature1, sizeof(uint32_t));
	FileSystem::read_next_file_chunk(&_file, &_signature2, sizeof(uint32_t));

	bool sig1correct = (_signature1 == SIGNATURE_1);
	bool sig2correct = ((_signature2 == SIGNATURE_2_2X_PRE_RELEASE)||
						(_signature2 == SIGNATURE_2_2X_POST_RELEASE));

	if(sig1correct && sig2correct) {
		return (DecryptionResult::SUCCESS);
	}

	return (DecryptionResult::SIGNATURE_ERROR);
}

void KeePassReader::_readHeader()
{
	uint8_t nfield;
	memset(_header, 0, sizeof(_header));

	//first 4 bytes in header = database version
	FileSystem::read_next_file_chunk(&_file, &_signature3, sizeof(uint32_t));

	//data in header organized as follows:
	//1 byte = number of header field
	//2 bytes = size of header filed
	//[size of header filed] bytes
	for(int i = 0; i < HEADER_FIELD_COUNT; i++) {
		FileSystem::read_next_file_chunk(&_file, &nfield, sizeof(uint8_t));

		if (nfield < HEADER_FIELD_COUNT) {
			FileSystem::read_next_file_chunk(&_file, &_header[nfield].size, sizeof(uint16_t));
			FileSystem::read_next_file_chunk(&_file, _header[nfield].data, _header[nfield].size);
		}
		if(nfield == END_OF_HEADER) {
			break;
		}
	}
}

void KeePassReader::_makeMasterKey(uint8_t *key, uint32_t key_len)
{
	uint8_t hash1[HASH_LENGTH];
	uint8_t hash2[HASH_LENGTH];

	KeePassCrypto::evalSHA256(key, key_len, hash1);
	KeePassCrypto::evalSHA256(hash1, HASH_LENGTH, hash2);

	_makeKeyRoutine(hash2);
}

void KeePassReader::_makeMasterKey(uint8_t *pass, uint8_t *keyfile, uint32_t pass_len, uint32_t keylile_len)
{
	uint8_t composite[COMPOSITE_KEY_LENGTH];
	uint8_t composite_hash[HASH_LENGTH];

	//getting hash of a password and key file
	KeePassCrypto::evalSHA256(pass, pass_len, &composite[0]);
	KeePassCrypto::evalSHA256(keyfile, keylile_len, &composite[HASH_LENGTH]);

	//getting hash of a composite key
	KeePassCrypto::evalSHA256(composite, COMPOSITE_KEY_LENGTH, composite_hash);

	//the main make key routine is the same as for keepass v.1
	_makeKeyRoutine(composite_hash);
}

void KeePassReader::_makeKeyRoutine(uint8_t *key_hash)
{
	uint8_t final_key[MASTER_KEY_LENGTH_2X];
	memcpy(&final_key[0], _header[MASTER_SEED].data, _header[MASTER_SEED].size);

	uint16_t rounds = (*(uint16_t*)_header[TRANSFORM_ROUNDS].data);

	KeePassCrypto::encrypt_AES_EBC(_header[TRANSFORM_SEED].data, key_hash, HASH_LENGTH, rounds);

	KeePassCrypto::evalSHA256(key_hash, HASH_LENGTH, &final_key[_header[MASTER_SEED].size]);
	KeePassCrypto::evalSHA256(final_key, MASTER_KEY_LENGTH_2X, _master_key);
}

DecryptionResult KeePassReader::_decrypt()
{
	uint8_t hash[HASH_LENGTH];
	memset(_decrypted_data, 0, MAX_DATABASE_SIZE_IN_BYTES);

	_file_len = _file.fsize - FileSystem::get_file_tell(&_file);
	FileSystem::read_next_file_chunk(&_file, _decrypted_data, _file_len);
	if (FileSystem::close_file(&_file) != FR_OK) {
		return (DB_FILE_ERROR);
	}

	KeePassCrypto::decrypt_AES_CBC(_master_key, _header[ENCRYPTION_IV].data, _decrypted_data, _file_len);

	if (memcmp(_header[STREAM_START_BYTES].data, _decrypted_data, _header[STREAM_START_BYTES].size)) {
		return (MASTER_KEY_ERROR);
	}

	_file_len -= _header[STREAM_START_BYTES].size;
	return (_process_blocks());
}

DecryptionResult KeePassReader::_process_blocks()
{
	uint8_t hash[HASH_LENGTH];
	uint32_t data_size = 0;
	uint32_t offset = _header[STREAM_START_BYTES].size;
	BlockDataHeader block_data;

	while (_file_len > sizeof(BlockDataHeader))
	{
		memcpy(&block_data, &_decrypted_data[offset], sizeof(BlockDataHeader));

		if (block_data.blockDataSize == 0) {
			break;
		}

		KeePassCrypto::evalSHA256(&_decrypted_data[offset + sizeof(BlockDataHeader)], block_data.blockDataSize, hash);

		if(!memcmp(block_data.blockDataHash, hash, HASH_LENGTH)) {
			memcpy(&_decrypted_data[data_size], &_decrypted_data[offset + sizeof(BlockDataHeader)], block_data.blockDataSize);
			offset += sizeof(BlockDataHeader) + block_data.blockDataSize;
			data_size += block_data.blockDataSize;
			_file_len = (_file_len - sizeof(BlockDataHeader) - block_data.blockDataSize);
		}

		else {
			return (DATA_HASH_ERROR);
		}
	}

	_decrypted_data[data_size++] = EOF;
	memset(&_decrypted_data[data_size], 0, (MAX_DATABASE_SIZE_IN_BYTES - data_size));
	_decrypt_passwords();
	return (SUCCESS);
}

void KeePassReader::_decrypt_passwords()
{
	uint32_t decoded_len = 0;
	uint8_t hash[HASH_LENGTH];
	uint8_t decrypted_pass[MAX_PASSWORD_SIZE_IN_BYTES] = {0};

	KeePassCrypto::evalSHA256(_header[PROTECTED_STREAM_KEY].data, _header[PROTECTED_STREAM_KEY].size, hash);
	KeePassCrypto::init_Salsa20(hash, IV_SALSA);

	if (_tree == nullptr) {
		_tree = mxmlLoadString(NULL,
							   (const char*)_decrypted_data,
							   MXML_OPAQUE_CALLBACK);

		mxml_node_t *last_node = _tree;
		mxml_node_t *curr_node;

		while(true)
		{
			curr_node = mxmlFindElement(last_node, _tree, "Value", "Protected", "True", MXML_DESCEND);

			if (curr_node == NULL) {
				break;
			}

			memset(decrypted_pass, 0, MAX_PASSWORD_SIZE_IN_BYTES);
			uint32_t len = strlen(curr_node->child->value.opaque);

			Base64::Decode(curr_node->child->value.opaque, len, (char*)decrypted_pass, len, &decoded_len);
			KeePassCrypto::eval_Salsa20(decrypted_pass, decoded_len);

			// curr_node->value.element.attrs[0].value = (char*)"False";
			std::strcpy(curr_node->child->value.opaque, (char*)decrypted_pass);

			last_node = curr_node;
		}
	}
}

mxml_node_t* KeePassReader::get_xml()
{
	return (_tree);
}

void KeePassReader::set_password(const char* pass, uint32_t len)
{
	if (len > 0) {
		memcpy(_pass, pass, len);
		_pass_len = len;
	}
	else {
		_pass_len = 0;
	}
}

DecryptionResult KeePassReader::decrypt_database(const char* db_name)
{
	//TODO: move to another place
	_credentials = KeePassCredentials::PASSWORD;
	////////////////////////////////////////////

	DecryptionResult result;

	if (FileSystem::open_file_to_read(&_file, db_name) != FR_OK) {
		return (DB_FILE_ERROR);
	}

	result = _checkKeePassVersion();
	if (result != SUCCESS) {
		FileSystem::close_file(&_file);
		return (result);
	}

	if (_credentials == KeePassCredentials::NOT_SELECTED) {
		FileSystem::close_file(&_file);
		return (CREDENTIALS_ERROR);
	}

	_readHeader();

	if (_credentials == KeePassCredentials::PASSWORD)
		_makeMasterKey(_pass, _pass_len);

	if (_credentials == KeePassCredentials::KEY_FILE)
		_makeMasterKey(_keyf, _keyf_len);

	if (_credentials == KeePassCredentials::PASSWORD_AND_KEY_FILE)
		_makeMasterKey(_pass, _keyf, _pass_len, _keyf_len );

	return (_decrypt());
}
