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

#ifndef USB_USB_DEVICE_USB_PACKAGE_H_
#define USB_USB_DEVICE_USB_PACKAGE_H_

#include <cstddef>
#include <cstring>

#include <keys/Key.h>

using namespace Keys;
using std::size_t;
using std::memset;

namespace UsbPackages {

typedef uint8_t UsbRawData;

constexpr static UsbRawData USB_EMPTY_FIELD = 0;

constexpr static size_t USB_PACKAGE_LENGTH = 8;
constexpr static size_t USB_PACKAGE_SPECIAL_FIELDS_LENGTH = 2;
constexpr static size_t USB_PACKAGE_KEY_FIELDS_LENGTH = 6;

#pragma pack(push, 1)
struct UsbPackage
{
	UsbSpecialKeySequence special;
	UsbRawData reserved;
	UsbKey key[USB_PACKAGE_KEY_FIELDS_LENGTH];

	friend bool operator==(const UsbPackage& lhs, const UsbPackage& rhs) {
		return (memcmp(&lhs, &rhs, UsbPackages::USB_PACKAGE_LENGTH) == 0);
	}

	friend bool operator!=(const UsbPackage& lhs, const UsbPackage& rhs) {
		return !(memcmp(&lhs, &rhs, UsbPackages::USB_PACKAGE_LENGTH) == 0);
	}

	UsbRawData* data() {
		return reinterpret_cast<UsbRawData*>(this);
	}

	const UsbRawData* data() const {
		return reinterpret_cast<const UsbRawData*>(this);
	}

	constexpr size_t length() const {
		return USB_PACKAGE_LENGTH;
	}

	void clear() {
		memset(data(), USB_EMPTY_FIELD, length());
	}
};
#pragma pack(pop)

using UsbPackageConst = const UsbPackage;

static UsbPackageConst ZERO_PACKAGE = {{USB_EMPTY_FIELD}, USB_EMPTY_FIELD,
		UsbKey::NOT_A_KEY, UsbKey::NOT_A_KEY, UsbKey::NOT_A_KEY,
		UsbKey::NOT_A_KEY, UsbKey::NOT_A_KEY, UsbKey::NOT_A_KEY};

} // namespace UsbPackage

#endif /* USB_USB_DEVICE_USB_PACKAGE_H_ */
