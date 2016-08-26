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

#ifndef CRYPTO_H
#define CRYPTO_H

#include <lib/crypto/base64.h>
#include <lib/crypto/salsa20.h>
#include <stdint.h>
#include <string.h>
#include <libopencm3/stm32/crypto.h>
extern "C" {
#include <lib/crypto/sha2.h>
}

namespace KeepAss
{
	class KeePassCrypto
	{
	public:
		KeePassCrypto() {};
		static void evalSHA256(const uint8_t *data, uint32_t len, uint8_t *hash);
		static void encrypt_AES_EBC(uint8_t *key, uint8_t *data, uint32_t data_len, uint32_t cycles);
		static void decrypt_AES_CBC(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t data_len);
		static void init_Salsa20(uint8_t *key, const uint8_t *iv);
		static void eval_Salsa20(uint8_t *input, uint32_t length);
	};
}
#endif
