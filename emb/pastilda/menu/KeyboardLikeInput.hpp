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

#ifndef MENU_KEYBOARDLIKEINPUT_HPP_
#define MENU_KEYBOARDLIKEINPUT_HPP_

#include <cstddef>
#include <cstring>

#include <usb_package.h>
#include <menu/UsbPackageFactory.h>

namespace Logic {

using namespace UsbPackages;

using std::size_t;
using std::memcmp;


// DESCRIPTION
// Keyboard have strange logic. If you press keys 1, 2 and 3 simultaneously
// then you will see input package like {0, 0, 0, 1, 2, 3, 0, 0, 0}. But if you
// send 1, 2 and 3 one by one very fast, then you'll see packages like:
//   1. {0, 0, 0, 1, 0, 0, 0, 0, 0}
//   2. {0, 0, 0, 1, 2, 0, 0, 0, 0}
//   3. {0, 0, 0, 2, 3, 0, 0, 0, 0}
//   4. {0, 0, 0, 3, 0, 0, 0, 0, 0}
// So this class converts this strange logic to linear.

template<typename T>
class KeyboardLikeInput {
public:
	using KeyBuffer = T;

	explicit KeyboardLikeInput(KeyBuffer* buffer
#ifdef DEBUG
			, PackageFactory* f
#endif
	);
	~KeyboardLikeInput();

	void process(UsbPackageConst* packagePtr);

private:
#ifdef DEBUG
	PackageFactory* _f;
#endif

	KeyBuffer* _buffer;

	UsbPackageConst* _inputPackagePtr;
	UsbPackage _lastPackage;

	bool _ignoreKeys;

	void _storeKey(Key& key);
	void _storeInputPackage();
	void _storeInputPackageSeries();

	void _resetInput();
};

template<typename T>
KeyboardLikeInput<T>::KeyboardLikeInput(KeyBuffer* buffer
#ifdef DEBUG
		, PackageFactory* f
#endif
) :
#ifdef DEBUG
	_f(f),
#endif
	_buffer(buffer),
	_inputPackagePtr(nullptr),
	_ignoreKeys(false)
{
	_resetInput();
}

template<typename T>
KeyboardLikeInput<T>::~KeyboardLikeInput()
{ }

template<typename T>
void KeyboardLikeInput<T>::process(UsbPackages::UsbPackageConst* packagePtr)
{
	_inputPackagePtr = packagePtr;

	if (*_inputPackagePtr != ZERO_PACKAGE) {
		if (_lastPackage == ZERO_PACKAGE) {
			_storeInputPackage();
#ifdef DEBUG
		_f->processData(" s+", 3);
		_f->generatePackage((uint8_t*)_inputPackagePtr);
#endif
		}
		else {
			if (_inputPackagePtr->key[2] != UsbKey::NOT_A_KEY) {
				_ignoreKeys = true;
			}

			if (_ignoreKeys == false) {
				_storeInputPackageSeries();
			}
#ifdef DEBUG
		_f->processData(" o+", 3);
		_f->generatePackage((uint8_t*)_inputPackagePtr);
#endif
		}
	}
	else {
		_resetInput();
	}

	_lastPackage = *_inputPackagePtr;
}

template<typename T>
void KeyboardLikeInput<T>::_storeInputPackage()
{
	for (size_t i = 0;
		 i < UsbPackages::USB_PACKAGE_KEY_FIELDS_LENGTH;
		 ++i)
	{
		Key key(_inputPackagePtr->key[i], _inputPackagePtr->special);
		_storeKey(key);
	}
}

template<typename T>
void KeyboardLikeInput<T>::_storeInputPackageSeries()
{
	for (size_t i = 0;
		 i < UsbPackages::USB_PACKAGE_KEY_FIELDS_LENGTH;
		 ++i)
	{
		Key key(_inputPackagePtr->key[i], _inputPackagePtr->special);
		AsciiCode compareValue = static_cast<AsciiCode>(_buffer->back());
		AsciiCodeType compareValueCode = _buffer->back();

		if (isAsciiShifted(compareValue)) {
			compareValueCode =
				static_cast<AsciiCodeType>(
						getAsciiBackShifted(compareValue)
					);
		}

		if (key.getAsciiCode() != compareValueCode) {
			_storeKey(key);
		}
	}
}

template<typename T>
void KeyboardLikeInput<T>::_storeKey(Key& key)
{
	if (key == UsbKey::KEY_BACKSPACE) {
		if (_buffer->empty() == false) {
			_buffer->pop_back();
		}
	}
	else if (key.isControl() == false &&
			 key.getUsbKey() != UsbKey::NOT_A_KEY)
	{
		if (_buffer->full() == false) {
			_buffer->push_back(key.getAsciiCode());
		}
	}
}

template<typename T>
void KeyboardLikeInput<T>::_resetInput()
{
	_lastPackage = ZERO_PACKAGE;
	_ignoreKeys = false;
}

} /* namespace Logic */

#endif /* MENU_KEYBOARDLIKEINPUT_HPP_ */
