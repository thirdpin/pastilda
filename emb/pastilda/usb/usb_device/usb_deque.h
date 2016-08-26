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

#ifndef USB_USB_DEVICE_USB_DEQUE_H_
#define USB_USB_DEVICE_USB_DEQUE_H_

#include <cstring>
#include <cstddef>

#include <etl/deque.h>

#include <keys/Key.h>
#include <usb_package.h>

using namespace Keys;
using std::size_t;
using std::memcmp;

namespace UsbPackages {

template <size_t USB_PACKAGE_DEQUE_SIZE>
class UsbDeque
{
public:
	using impl = etl::deque<UsbPackage, USB_PACKAGE_DEQUE_SIZE>;
};


template <size_t USB_PACKAGE_DEQUE_SIZE>
class UsbDequeSave : public UsbDeque<USB_PACKAGE_DEQUE_SIZE>::impl
{
public:
	UsbDequeSave() :
		UsbDeque<USB_PACKAGE_DEQUE_SIZE>::impl(),
		dequeLocked(true)
	{ }

	void lock() {
		dequeLocked = true;
	}

	void unlock() {
		dequeLocked = false;
	}

	bool isPopOnly() {
		return isLocked();
	}

	bool isLocked() {
		return dequeLocked;
	}

private:
	bool dequeLocked;
};


static constexpr size_t USB_DEQUE_STANDART_SIZE = 500;
using UsbDequeStandart = UsbDequeSave<USB_DEQUE_STANDART_SIZE>;

} /* namespace UsbPackages */


#endif /* USB_USB_DEVICE_USB_DEQUE_H_ */
