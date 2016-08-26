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

#include "keepass_crypto.h"

using namespace KeepAss;

void KeePassCrypto::evalSHA256(const uint8_t *data, uint32_t len, uint8_t *hash)
{
	cf_sha256_context ctx;
	cf_sha256_init(&ctx);
	cf_sha256_update(&ctx, data, len);
	cf_sha256_digest_final(&ctx, hash);
}

void KeePassCrypto::encrypt_AES_EBC(uint8_t *key, uint8_t *data, uint32_t data_len, uint32_t cycles)
{
	crypto_set_key(CRYPTO_KEY_256BIT, key);
	crypto_set_algorithm(ENCRYPT_AES_ECB);
	crypto_set_datatype(CRYPTO_DATA_8BIT);
	crypto_start();

	for (int i = 0; i < cycles; i++) {
		crypto_process_block((uint32_t*)data, (uint32_t*)data, data_len / sizeof(uint32_t));
	}

	crypto_stop();
}

void KeePassCrypto::decrypt_AES_CBC(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t data_len)
{
	crypto_set_key(CRYPTO_KEY_256BIT, key);
	crypto_set_iv(iv);
	crypto_set_datatype(CRYPTO_DATA_8BIT);
	crypto_set_algorithm(crypto_mode::DECRYPT_AES_CBC);
	crypto_start();
	crypto_process_block((uint32_t*)data, (uint32_t*)data, data_len / sizeof(uint32_t));
	crypto_stop();
}

static Salsa20 salsa;
static uint8_t key_stream[Salsa20::BLOCK_SIZE];
static uint8_t stream_pointer;

void KeePassCrypto::init_Salsa20(uint8_t *key, const uint8_t *iv)
{
	salsa.setKey(key);
	salsa.setIv(iv);
	stream_pointer = 0;
	memset(key_stream, 0, Salsa20::BLOCK_SIZE);
	salsa.generateKeyStream(key_stream);
}

void KeePassCrypto::eval_Salsa20(uint8_t *input, uint32_t length)
{
	for (int i = 0; i < length; i++) {
		input[i] = input[i] ^ key_stream[stream_pointer];
		stream_pointer++;

		if (stream_pointer >= Salsa20::BLOCK_SIZE) {
			memset(key_stream, 0, Salsa20::BLOCK_SIZE);
			salsa.generateKeyStream(key_stream);
			stream_pointer = 0;
		}
	}
}
