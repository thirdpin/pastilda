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

#ifndef KEYS_USBKEYCODES_H_
#define KEYS_USBKEYCODES_H_

#include <cstddef>
#include <cstdint>

namespace Keys {

typedef uint8_t UsbKeyCodeType;

enum class UsbKey : UsbKeyCodeType {
	NOT_A_KEY = 0x00,
	KEY_ERRORROLLOVER = 0x01,
	KEY_POSTFAIL = 0x02,
	KEY_ERRORUNDEFINED = 0x03,
	KEY_A = 0x04,
	KEY_B = 0x05,
	KEY_C = 0x06,
	KEY_D = 0x07,
	KEY_E = 0x08,
	KEY_F = 0x09,
	KEY_G = 0x0A,
	KEY_H = 0x0B,
	KEY_I = 0x0C,
	KEY_J = 0x0D,
	KEY_K = 0x0E,
	KEY_L = 0x0F,
	KEY_M = 0x10,
	KEY_N = 0x11,
	KEY_O = 0x12,
	KEY_P = 0x13,
	KEY_Q = 0x14,
	KEY_R = 0x15,
	KEY_S = 0x16,
	KEY_T = 0x17,
	KEY_U = 0x18,
	KEY_V = 0x19,
	KEY_W = 0x1A,
	KEY_X = 0x1B,
	KEY_Y = 0x1C,
	KEY_Z = 0x1D,
	KEY_1_EXCLAMATION_MARK = 0x1E,
	KEY_2_AT = 0x1F,
	KEY_3_NUMBER_SIGN = 0x20,
	KEY_4_DOLLAR = 0x21,
	KEY_5_PERCENT = 0x22,
	KEY_6_CARET = 0x23,
	KEY_7_AMPERSAND = 0x24,
	KEY_8_ASTERISK = 0x25,
	KEY_9_OPARENTHESIS = 0x26,
	KEY_0_CPARENTHESIS = 0x27,
	KEY_ENTER = 0x28,
	KEY_ESCAPE = 0x29,
	KEY_BACKSPACE = 0x2A,
	KEY_TAB = 0x2B,
	KEY_SPACEBAR = 0x2C,
	KEY_MINUS_UNDERSCORE = 0x2D,
	KEY_EQUAL_PLUS = 0x2E,
	KEY_OBRACKET_AND_OBRACE = 0x2F,
	KEY_CBRACKET_AND_CBRACE = 0x30,
	KEY_BACKSLASH_VERTICAL_BAR = 0x31,
	KEY_NONUS_NUMBER_SIGN_TILDE = 0x32,
	KEY_SEMICOLON_COLON = 0x33,
	KEY_SINGLE_AND_DOUBLE_QUOTE = 0x34,
	KEY_GRAVE_ACCENT_AND_TILDE = 0x35,
	KEY_COMMA_AND_LESS = 0x36,
	KEY_DOT_GREATER = 0x37,
	KEY_SLASH_QUESTION = 0x38,
	KEY_CAPS_LOCK = 0x39,
	KEY_F1 = 0x3A,
	KEY_F2 = 0x3B,
	KEY_F3 = 0x3C,
	KEY_F4 = 0x3D,
	KEY_F5 = 0x3E,
	KEY_F6 = 0x3F,
	KEY_F7 = 0x40,
	KEY_F8 = 0x41,
	KEY_F9 = 0x42,
	KEY_F10 = 0x43,
	KEY_F11 = 0x44,
	KEY_F12 = 0x45,
	KEY_PRINTSCREEN = 0x46,
	KEY_SCROLL_LOCK = 0x47,
	KEY_PAUSE = 0x48,
	KEY_INSERT = 0x49,
	KEY_HOME = 0x4A,
	KEY_PAGEUP = 0x4B,
	KEY_DELETE = 0x4C,
	KEY_END1 = 0x4D,
	KEY_PAGEDOWN = 0x4E,
	KEY_RIGHTARROW = 0x4F,
	KEY_LEFTARROW = 0x50,
	KEY_DOWNARROW = 0x51,
	KEY_UPARROW = 0x52,
	KEY_KEYPAD_NUM_LOCK_AND_CLEAR = 0x53,
	KEY_KEYPAD_SLASH = 0x54,
	KEY_KEYPAD_ASTERIKS = 0x55,
	KEY_KEYPAD_MINUS = 0x56,
	KEY_KEYPAD_PLUS = 0x57,
	KEY_KEYPAD_ENTER = 0x58,
	KEY_KEYPAD_1_END = 0x59,
	KEY_KEYPAD_2_DOWN_ARROW = 0x5A,
	KEY_KEYPAD_3_PAGEDN = 0x5B,
	KEY_KEYPAD_4_LEFT_ARROW = 0x5C,
	KEY_KEYPAD_5 = 0x5D,
	KEY_KEYPAD_6_RIGHT_ARROW = 0x5E,
	KEY_KEYPAD_7_HOME = 0x5F,
	KEY_KEYPAD_8_UP_ARROW = 0x60,
	KEY_KEYPAD_9_PAGEUP = 0x61,
	KEY_KEYPAD_0_INSERT = 0x62,
	KEY_KEYPAD_DECIMAL_SEPARATOR_DELETE = 0x63,
	KEY_NONUS_BACK_SLASH_VERTICAL_BAR = 0x64,
	KEY_APPLICATION = 0x65,
	KEY_POWER = 0x66,
	KEY_KEYPAD_EQUAL = 0x67,
	KEY_F13 = 0x68,
	KEY_F14 = 0x69,
	KEY_F15 = 0x6A,
	KEY_F16 = 0x6B,
	KEY_F17 = 0x6C,
	KEY_F18 = 0x6D,
	KEY_F19 = 0x6E,
	KEY_F20 = 0x6F,
	KEY_F21 = 0x70,
	KEY_F22 = 0x71,
	KEY_F23 = 0x72,
	KEY_F24 = 0x73,
	KEY_EXECUTE = 0x74,
	KEY_HELP = 0x75,
	KEY_MENU = 0x76,
	KEY_SELECT = 0x77,
	KEY_STOP = 0x78,
	KEY_AGAIN = 0x79,
	KEY_UNDO = 0x7A,
	KEY_CUT = 0x7B,
	KEY_COPY = 0x7C,
	KEY_PASTE = 0x7D,
	KEY_FIND = 0x7E,
	KEY_MUTE = 0x7F,
	KEY_VOLUME_UP = 0x80,
	KEY_VOLUME_DOWN = 0x81,
	KEY_LOCKING_CAPS_LOCK = 0x82,
	KEY_LOCKING_NUM_LOCK = 0x83,
	KEY_LOCKING_SCROLL_LOCK = 0x84,
	KEY_KEYPAD_COMMA = 0x85,
	KEY_KEYPAD_EQUAL_SIGN = 0x86,
	KEY_INTERNATIONAL1 = 0x87,
	KEY_INTERNATIONAL2 = 0x88,
	KEY_INTERNATIONAL3 = 0x89,
	KEY_INTERNATIONAL4 = 0x8A,
	KEY_INTERNATIONAL5 = 0x8B,
	KEY_INTERNATIONAL6 = 0x8C,
	KEY_INTERNATIONAL7 = 0x8D,
	KEY_INTERNATIONAL8 = 0x8E,
	KEY_INTERNATIONAL9 = 0x8F,
	KEY_LANG1 = 0x90,
	KEY_LANG2 = 0x91,
	KEY_LANG3 = 0x92,
	KEY_LANG4 = 0x93,
	KEY_LANG5 = 0x94,
	KEY_LANG6 = 0x95,
	KEY_LANG7 = 0x96,
	KEY_LANG8 = 0x97,
	KEY_LANG9 = 0x98,
	KEY_ALTERNATE_ERASE = 0x99,
	KEY_SYSREQ = 0x9A,
	KEY_CANCEL = 0x9B,
	KEY_CLEAR = 0x9C,
	KEY_PRIOR = 0x9D,
	KEY_RETURN = 0x9E,
	KEY_SEPARATOR = 0x9F,
	KEY_OUT = 0xA0,
	KEY_OPER = 0xA1,
	KEY_CLEAR_AGAIN = 0xA2,
	KEY_CRSEL = 0xA3,
	KEY_EXSEL = 0xA4,
	KEY_KEYPAD_00 = 0xB0,
	KEY_KEYPAD_000 = 0xB1,
	KEY_THOUSANDS_SEPARATOR = 0xB2,
	KEY_DECIMAL_SEPARATOR = 0xB3,
	KEY_CURRENCY_UNIT = 0xB4,
	KEY_CURRENCY_SUB_UNIT = 0xB5,
	KEY_KEYPAD_OPARENTHESIS = 0xB6,
	KEY_KEYPAD_CPARENTHESIS = 0xB7,
	KEY_KEYPAD_OBRACE = 0xB8,
	KEY_KEYPAD_CBRACE = 0xB9,
	KEY_KEYPAD_TAB = 0xBA,
	KEY_KEYPAD_BACKSPACE = 0xBB,
	KEY_KEYPAD_A = 0xBC,
	KEY_KEYPAD_B = 0xBD,
	KEY_KEYPAD_C = 0xBE,
	KEY_KEYPAD_D = 0xBF,
	KEY_KEYPAD_E = 0xC0,
	KEY_KEYPAD_F = 0xC1,
	KEY_KEYPAD_XOR = 0xC2,
	KEY_KEYPAD_CARET = 0xC3,
	KEY_KEYPAD_PERCENT = 0xC4,
	KEY_KEYPAD_LESS = 0xC5,
	KEY_KEYPAD_GREATER = 0xC6,
	KEY_KEYPAD_AMPERSAND = 0xC7,
	KEY_KEYPAD_LOGICAL_AND = 0xC8,
	KEY_KEYPAD_VERTICAL_BAR = 0xC9,
	KEY_KEYPAD_LOGICAL_OR = 0xCA,
	KEY_KEYPAD_COLON = 0xCB,
	KEY_KEYPAD_NUMBER_SIGN = 0xCC,
	KEY_KEYPAD_SPACE = 0xCD,
	KEY_KEYPAD_AT = 0xCE,
	KEY_KEYPAD_EXCLAMATION_MARK = 0xCF,
	KEY_KEYPAD_MEMORY_STORE = 0xD0,
	KEY_KEYPAD_MEMORY_RECALL = 0xD1,
	KEY_KEYPAD_MEMORY_CLEAR = 0xD2,
	KEY_KEYPAD_MEMORY_ADD = 0xD3,
	KEY_KEYPAD_MEMORY_SUBTRACT = 0xD4,
	KEY_KEYPAD_MEMORY_MULTIPLY = 0xD5,
	KEY_KEYPAD_MEMORY_DIVIDE = 0xD6,
	KEY_KEYPAD_PLUSMINUS = 0xD7,
	KEY_KEYPAD_CLEAR = 0xD8,
	KEY_KEYPAD_CLEAR_ENTRY = 0xD9,
	KEY_KEYPAD_BINARY = 0xDA,
	KEY_KEYPAD_OCTAL = 0xDB,
	KEY_KEYPAD_DECIMAL = 0xDC,
	KEY_KEYPAD_HEXADECIMAL = 0xDD,
	KEY_LEFTCONTROL = 0xE0,
	KEY_LEFTSHIFT = 0xE1,
	KEY_LEFTALT = 0xE2,
	KEY_LEFT_GUI = 0xE3,
	KEY_RIGHTCONTROL = 0xE4,
	KEY_RIGHTSHIFT = 0xE5,
	KEY_RIGHTALT = 0xE6,
	KEY_RIGHT_GUI = 0xE7
};

static constexpr std::size_t USB_SPECIAL_KEY_CODE_COUNT = 8;
enum class UsbSpecialKey : UsbKeyCodeType {
	NO_KEY		= 0x00,
	LEFT_CTRL   = 0x01,
	LEFT_SHIFT  = 0x02,
	LEFT_ALT    = 0x04,
	LEFT_GUI    = 0x08,
	RIGHT_CTRL  = 0x10,
	RIGHT_SHIFT = 0x20,
	RIGHT_ALT   = 0x40,
	RIGHT_GUI   = 0x80
};

#pragma pack(push, 1)
struct UsbSpecialKeySequence
{
	UsbKeyCodeType
		leftCtrl : 1,    // 00000001
		leftShift : 1,   // 00000010
		leftAlt : 1,     // 00000100
		leftGui : 1,     // 00001000
		rightCtrl : 1,   // 00010000
		rightShift : 1,  // 00100000
		rightAlt : 1,	 // 01000000
		rightGui : 1;	 // 10000000

	const UsbKeyCodeType getMask() const {
		return *reinterpret_cast<const UsbKeyCodeType*>(this);
	}

	void set(UsbKeyCodeType mask) {
		*reinterpret_cast<UsbKeyCodeType*>(this) = mask;
	}

	void clear() {
		set(0);
	}

	void set(UsbKeyCodeType key1,
			 UsbKeyCodeType key2,
			 UsbKeyCodeType key3 = 0,
			 UsbKeyCodeType key4 = 0,
			 UsbKeyCodeType key5 = 0,
			 UsbKeyCodeType key6 = 0,
			 UsbKeyCodeType key7 = 0,
			 UsbKeyCodeType key8 = 0)
	{
		set(key1 | key2 | key3 | key4 | key5 | key6 | key7 | key8);
	}

	void set(UsbSpecialKey key1,
			 UsbSpecialKey key2 = UsbSpecialKey::NO_KEY,
			 UsbSpecialKey key3 = UsbSpecialKey::NO_KEY,
			 UsbSpecialKey key4 = UsbSpecialKey::NO_KEY,
			 UsbSpecialKey key5 = UsbSpecialKey::NO_KEY,
			 UsbSpecialKey key6 = UsbSpecialKey::NO_KEY,
			 UsbSpecialKey key7 = UsbSpecialKey::NO_KEY,
			 UsbSpecialKey key8 = UsbSpecialKey::NO_KEY)
	{
		set(
			static_cast<UsbKeyCodeType>(key1) |
			static_cast<UsbKeyCodeType>(key2) |
			static_cast<UsbKeyCodeType>(key3) |
			static_cast<UsbKeyCodeType>(key4) |
			static_cast<UsbKeyCodeType>(key5) |
			static_cast<UsbKeyCodeType>(key6) |
			static_cast<UsbKeyCodeType>(key7) |
			static_cast<UsbKeyCodeType>(key8)
		);
	}

	void set(UsbSpecialKeySequence& seq) {
		set(seq.getMask());
	}

	bool isSetted(UsbSpecialKey key) {
		return ((static_cast<UsbKeyCodeType>(key) & getMask()) != 0);
	}

	template<typename T>
	UsbSpecialKeySequence& operator=(const T& src) {
		set(src);
		return *this;
	}

	bool operator==(UsbSpecialKey key) const {
		return (static_cast<UsbKeyCodeType>(key) == getMask());
	}

	bool operator==(UsbSpecialKeySequence seq) const {
		return (seq.getMask() == getMask());
	}
};
#pragma pack(pop)

}

#endif /* KEYS_USBKEYCODES_H_ */
