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

#ifndef KEYS_ASCIIKEYCODES_H_
#define KEYS_ASCIIKEYCODES_H_

#include <cstdint>

namespace Keys {

typedef uint8_t AsciiCodeType;

enum class AsciiCode : AsciiCodeType {
	NUL = 0x00, // Null char
	SOH = 0x01, // Start of Heading
	STX = 0x02, // Start of Text
	ETX = 0x03, // End of Text
	EOT = 0x04, // End of Transmission
	ENQ = 0x05, // Enquiry
	ACK = 0x06, // Acknowledgment
	BEL = 0x07, // Bell
	BACKSPACE = 0x08, // Back Space
	TAB = 0x09, // Horizontal Tab
	LF = 0x0A, // Line Feed
	VTAB = 0x0B, // Vertical Tab
	FF = 0x0C, // Form Feed
	CR = 0x0D, // Carriage Return
	SO = 0x0E, // Shift Out / X-On
	SI = 0x0F, // Shift In / X-Off
	DLE = 0x10, // Data Line Escape
	DC1 = 0x11, // Device Control 1 (oft. XON)
	DC2 = 0x12, // Device Control 2
	DC3 = 0x13, // Device Control 3 (oft. XOFF)
	DC4 = 0x14, // Device Control 4
	NAK = 0x15, // Negative Acknowledgement
	SYN = 0x16, // Synchronous Idle
	ETB = 0x17, // End of Transmit Block
	CAN = 0x18, // Cancel
	EM = 0x19, // End of Medium
	SUB = 0x1A, // Substitute
	ESC = 0x1B, // Escape
	FS = 0x1C, // File Separator
	GS = 0x1D, // Group Separator
	RS = 0x1E, // Record Separator
	US = 0x1F, // Unit Separator
	SPACE = 0x20, //  , Space
	EXCLAMATION_MARK = 0x21, // !, Exclamation mark
	DOUBLE_QUOTES = 0x22, // ", Double quotes (or speech marks)
	NUMBER = 0x23, // #, Number
	DOLLAR = 0x24, // $, Dollar
	PERCENT = 0x25, // %, Percent
	AMPERSAND = 0x26, // &, Ampersand
	SINGLE_QUOTE = 0x27, // ', Single quote
	OPEN_PARENTHESIS = 0x28, // (, Open parenthesis (or open bracket)
	CLOSE_PARENTHESIS = 0x29, // ), Close parenthesis (or close bracket)
	ASTERISK = 0x2A, // *, Asterisk
	PLUS = 0x2B, // +, Plus
	COMMA = 0x2C, // ,, Comma
	HYPHEN = 0x2D, // -, Hyphen
	PERIOD_OR_DOT = 0x2E, // ., Period, dot or full stop
	SLASH_OR_DIVIDE = 0x2F, // /, Slash or divide
	ZERO = 0x30, // 0, Zero
	ONE = 0x31, // 1, One
	TWO = 0x32, // 2, Two
	THREE = 0x33, // 3, Three
	FOUR = 0x34, // 4, Four
	FIVE = 0x35, // 5, Five
	SIX = 0x36, // 6, Six
	SEVEN = 0x37, // 7, Seven
	EIGHT = 0x38, // 8, Eight
	NINE = 0x39, // 9, Nine
	COLON = 0x3A, // :, Colon
	SEMICOLON = 0x3B, // ;, Semicolon
	LESS_THAN = 0x3C, // <, Less than (or open angled bracket)
	EQUALS = 0x3D, // =, Equals
	GREATER_THAN = 0x3E, // >, Greater than (or close angled bracket)
	QUESTION_MARK = 0x3F, // ?, Question mark
	AT_SYMBOL = 0x40, // @, At symbol
	UPPERCASE_A = 0x41, // A, Uppercase A
	UPPERCASE_B = 0x42, // B, Uppercase B
	UPPERCASE_C = 0x43, // C, Uppercase C
	UPPERCASE_D = 0x44, // D, Uppercase D
	UPPERCASE_E = 0x45, // E, Uppercase E
	UPPERCASE_F = 0x46, // F, Uppercase F
	UPPERCASE_G = 0x47, // G, Uppercase G
	UPPERCASE_H = 0x48, // H, Uppercase H
	UPPERCASE_I = 0x49, // I, Uppercase I
	UPPERCASE_J = 0x4A, // J, Uppercase J
	UPPERCASE_K = 0x4B, // K, Uppercase K
	UPPERCASE_L = 0x4C, // L, Uppercase L
	UPPERCASE_M = 0x4D, // M, Uppercase M
	UPPERCASE_N = 0x4E, // N, Uppercase N
	UPPERCASE_O = 0x4F, // O, Uppercase O
	UPPERCASE_P = 0x50, // P, Uppercase P
	UPPERCASE_Q = 0x51, // Q, Uppercase Q
	UPPERCASE_R = 0x52, // R, Uppercase R
	UPPERCASE_S = 0x53, // S, Uppercase S
	UPPERCASE_T = 0x54, // T, Uppercase T
	UPPERCASE_U = 0x55, // U, Uppercase U
	UPPERCASE_V = 0x56, // V, Uppercase V
	UPPERCASE_W = 0x57, // W, Uppercase W
	UPPERCASE_X = 0x58, // X, Uppercase X
	UPPERCASE_Y = 0x59, // Y, Uppercase Y
	UPPERCASE_Z = 0x5A, // Z, Uppercase Z
	OPENING_BRACKET = 0x5B, // [, Opening bracket
	BACKSLASH = 0x5C, // \, Backslash
	CLOSING_BRACKET = 0x5D, // ], Closing bracket
	CARET = 0x5E, // ^, Caret - circumflex
	UNDERSCORE = 0x5F, // _, Underscore
	GRAVE_ACCENT = 0x60, // `, Grave accent
	LOWERCASE_A = 0x61, // a, Lowercase a
	LOWERCASE_B = 0x62, // b, Lowercase b
	LOWERCASE_C = 0x63, // c, Lowercase c
	LOWERCASE_D = 0x64, // d, Lowercase d
	LOWERCASE_E = 0x65, // e, Lowercase e
	LOWERCASE_F = 0x66, // f, Lowercase f
	LOWERCASE_G = 0x67, // g, Lowercase g
	LOWERCASE_H = 0x68, // h, Lowercase h
	LOWERCASE_I = 0x69, // i, Lowercase i
	LOWERCASE_J = 0x6A, // j, Lowercase j
	LOWERCASE_K = 0x6B, // k, Lowercase k
	LOWERCASE_L = 0x6C, // l, Lowercase l
	LOWERCASE_M = 0x6D, // m, Lowercase m
	LOWERCASE_N = 0x6E, // n, Lowercase n
	LOWERCASE_O = 0x6F, // o, Lowercase o
	LOWERCASE_P = 0x70, // p, Lowercase p
	LOWERCASE_Q = 0x71, // q, Lowercase q
	LOWERCASE_R = 0x72, // r, Lowercase r
	LOWERCASE_S = 0x73, // s, Lowercase s
	LOWERCASE_T = 0x74, // t, Lowercase t
	LOWERCASE_U = 0x75, // u, Lowercase u
	LOWERCASE_V = 0x76, // v, Lowercase v
	LOWERCASE_W = 0x77, // w, Lowercase w
	LOWERCASE_X = 0x78, // x, Lowercase x
	LOWERCASE_Y = 0x79, // y, Lowercase y
	LOWERCASE_Z = 0x7A, // z, Lowercase z
	OPENING_BRACE = 0x7B, // {, Opening brace
	VERTICAL_BAR = 0x7C, // |, Vertical bar
	CLOSING_BRACE = 0x7D, // }, Closing brace
	TILDE_OR_EQUIVALENCY_SIGN = 0x7E, // ~, Equivalency sign - tilde
	DELETE = 0x7F // , Delete
};

}



#endif /* KEYS_ASCIIKEYCODES_H_ */
