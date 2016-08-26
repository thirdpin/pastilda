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

#ifndef KEYS_KEY_H_
#define KEYS_KEY_H_

#include <cstdint>

#include <UsbKeyCodes.h>
#include <AsciiKeyCodes.h>

namespace Keys {

AsciiCode getAsciiByUsbKey(UsbKey code);
UsbKey getUsbKeyByAscii(AsciiCode code);

AsciiCode getAsciiShifted(AsciiCode code);
AsciiCode getAsciiBackShifted(AsciiCode code);
bool isAsciiShifted(AsciiCode code);

class Key {
public:

	enum KeyType {
		CONTROL,
		SYBMOL
	};

	Key();
	Key(UsbKey key);
	Key(UsbKeyCodeType key, UsbKeyCodeType modifier);
	Key(UsbKeyCodeType key, UsbSpecialKey modifier);
	Key(UsbKey key, UsbKeyCodeType modifier);
	Key(UsbKey key, UsbSpecialKeySequence modifier);
	Key(UsbKey key, UsbSpecialKey modifier);
	Key(AsciiCodeType code);
	Key(AsciiCode code);
	~Key();

	void set(UsbKeyCodeType key, UsbKeyCodeType modifier);
	void set(UsbKey key, UsbSpecialKeySequence modifier);
	void set(UsbKey key, UsbSpecialKey modifier);
	void set(AsciiCodeType code);

	UsbKeyCodeType getUsbKeyCode() const {
		return static_cast<UsbKeyCodeType>(_usbKey);
	}

	UsbKey getUsbKey() const {
		return _usbKey;
	}

	UsbSpecialKeySequence getUsbKeyModifier() const {
		return _usbKeyModifier;
	}

	UsbKeyCodeType getUsbKeyModifierMask() {
		return _usbKeyModifier.getMask();
	}

	AsciiCodeType getAsciiCode() const {
		return static_cast<AsciiCodeType>(_asciiCode);
	}

	AsciiCode getAscii() const {
		return _asciiCode;
	}

	bool isControl() const {
		return (_keyType == CONTROL);
	}

	bool isShifted() const {
		return isAsciiShifted(_asciiCode);
	}

	bool operator==(Key& rhs) const {
		return (rhs._usbKey == _usbKey) &&
				(rhs._usbKeyModifier == _usbKeyModifier);
	}

	bool operator!=(Key& rhs) const {
		return !((rhs._usbKey == _usbKey) &&
				 (rhs._usbKeyModifier == _usbKeyModifier));
	}

	friend bool operator==(Key& lhs, UsbKey rhs) {
		return (lhs._usbKey == rhs);
	}

	friend bool operator!=(Key& lhs, UsbKey rhs) {
		return !(lhs._usbKey == rhs);
	}

	friend bool operator==(UsbKey rhs, Key& lhs) {
		return (lhs._usbKey == rhs);
	}

	friend bool operator==(Key& lhs, AsciiCode rhs) {
		return (lhs._asciiCode == rhs);
	}

	friend bool operator==(AsciiCode rhs, Key& lhs) {
		return (lhs._asciiCode == rhs);
	}

	Key& operator=(const Key& rhs) {
		_asciiCode = rhs.getAscii();
		_usbKey = rhs.getUsbKey();
		_usbKeyModifier = rhs.getUsbKeyModifier();
		_updateKeyType();
		return *this;
	}

	Key& operator=(const UsbKey& rhs) {
		set(rhs, UsbSpecialKey::NO_KEY);
		return *this;
	}

private:
	UsbSpecialKeySequence _usbKeyModifier;
	UsbKey _usbKey;

	AsciiCode _asciiCode;

	KeyType _keyType;

	void _checkAsciiShifted();
	void _updateKeyType();
};

} /* namespace Keys */

#endif /* KEYS_KEY_H_ */
