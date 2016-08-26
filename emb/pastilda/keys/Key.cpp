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

#include <UsbKeyCodes.h>
#include <AsciiKeyCodes.h>
#include <Key.h>

namespace {
using namespace Keys;

static AsciiCode usbKeyToAsciiCode[128] = {
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::UPPERCASE_A,
	AsciiCode::UPPERCASE_B,
	AsciiCode::UPPERCASE_C,
	AsciiCode::UPPERCASE_D,
	AsciiCode::UPPERCASE_E,
	AsciiCode::UPPERCASE_F,
	AsciiCode::UPPERCASE_G,
	AsciiCode::UPPERCASE_H,
	AsciiCode::UPPERCASE_I,
	AsciiCode::UPPERCASE_J,
	AsciiCode::UPPERCASE_K,
	AsciiCode::UPPERCASE_L,
	AsciiCode::UPPERCASE_M,
	AsciiCode::UPPERCASE_N,
	AsciiCode::UPPERCASE_O,
	AsciiCode::UPPERCASE_P,
	AsciiCode::UPPERCASE_Q,
	AsciiCode::UPPERCASE_R,
	AsciiCode::UPPERCASE_S,
	AsciiCode::UPPERCASE_T,
	AsciiCode::UPPERCASE_U,
	AsciiCode::UPPERCASE_V,
	AsciiCode::UPPERCASE_W,
	AsciiCode::UPPERCASE_X,
	AsciiCode::UPPERCASE_Y,
	AsciiCode::UPPERCASE_Z,
	AsciiCode::EXCLAMATION_MARK,
	AsciiCode::AT_SYMBOL,
	AsciiCode::NUMBER,
	AsciiCode::DOLLAR,
	AsciiCode::PERCENT,
	AsciiCode::CARET,
	AsciiCode::AMPERSAND,
	AsciiCode::ASTERISK,
	AsciiCode::OPEN_PARENTHESIS,
	AsciiCode::CLOSE_PARENTHESIS,
	AsciiCode::LF,
	AsciiCode::ESC,
	AsciiCode::BACKSPACE,
	AsciiCode::TAB,
	AsciiCode::SPACE,
	AsciiCode::UNDERSCORE,
	AsciiCode::EQUALS,
	AsciiCode::OPENING_BRACKET,
	AsciiCode::CLOSING_BRACKET,
	AsciiCode::BACKSLASH,
	AsciiCode::NUL, // KEY_NONUS_NUMBER_SIGN_TILDE
	AsciiCode::SEMICOLON,
	AsciiCode::SINGLE_QUOTE,
	AsciiCode::GRAVE_ACCENT,
	AsciiCode::COMMA,
	AsciiCode::PERIOD_OR_DOT,
	AsciiCode::SLASH_OR_DIVIDE,
	AsciiCode::NUL, // Caps Lock
	AsciiCode::NUL, // F1
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL, // F12
	AsciiCode::NUL, // KEY_PRINTSCREEN
	AsciiCode::NUL, // KEY_SCROLL_LOCK
	AsciiCode::NUL, // KEY_PAUSE
	AsciiCode::NUL, // KEY_INSERT
	AsciiCode::NUL, // KEY_HOME
	AsciiCode::NUL, // KEY_PAGEUP
	AsciiCode::DELETE,
	AsciiCode::NUL, // KEY_END1
	AsciiCode::NUL, // KEY_PAGEDOWN
	AsciiCode::NUL, // KEY_RIGHTARROW
	AsciiCode::NUL, // KEY_LEFTARROW
	AsciiCode::NUL, // KEY_DOWNARROW
	AsciiCode::NUL, // KEY_UPARROW
	AsciiCode::NUL, // KEY_KEYPAD_NUM_LOCK_AND_CLEAR
	AsciiCode::SLASH_OR_DIVIDE,
	AsciiCode::ASTERISK,
	AsciiCode::HYPHEN,
	AsciiCode::PLUS,
	AsciiCode::LF,
	AsciiCode::ONE,
	AsciiCode::TWO,
	AsciiCode::THREE,
	AsciiCode::FOUR,
	AsciiCode::FIVE,
	AsciiCode::SIX,
	AsciiCode::SEVEN,
	AsciiCode::EIGHT,
	AsciiCode::NINE,
	AsciiCode::ZERO,
	AsciiCode::PERIOD_OR_DOT,
	AsciiCode::SLASH_OR_DIVIDE,
	AsciiCode::NUL,  // KEY_APPLICATION
	AsciiCode::ASTERISK,
	AsciiCode::EQUALS,
	AsciiCode::NUL,  // F13
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL, // F24
	AsciiCode::NUL, // KEY_EXECUTE
	AsciiCode::NUL, // KEY_HELP
	AsciiCode::NUL, // KEY_MENU
	AsciiCode::NUL, // KEY_SELECT
	AsciiCode::NUL, // KEY_STOP
	AsciiCode::NUL, // KEY_AGAIN
	AsciiCode::NUL, // KEY_UNDO
	AsciiCode::NUL, // KEY_CUT
	AsciiCode::NUL, // KEY_COPY
	AsciiCode::NUL, // KEY_PASTE
	AsciiCode::NUL // KEY_FIND
};

constexpr static size_t SHIFT_FOR_SHIFTED_ASCII_CODES = 0x21;
static AsciiCode asciiShifted[128 - SHIFT_FOR_SHIFTED_ASCII_CODES] = {
	AsciiCode::NUL,				  // EXCLAMATION_MARK
	AsciiCode::NUL,     		  // DOUBLE_QUOTES
	AsciiCode::NUL,               // NUMBER
	AsciiCode::NUL,  		      // DOLLAR
	AsciiCode::NUL,  		      // PERCENT
	AsciiCode::NUL,  	          // AMPERSAND
	AsciiCode::DOUBLE_QUOTES,	  // SINGLE_QUOTE
	AsciiCode::NUL,				  // OPEN_PARENTHESIS
	AsciiCode::NUL,				  // CLOSE_PARENTHESIS
	AsciiCode::NUL,				  // ASTERISK
	AsciiCode::NUL,			      // PLUS
	AsciiCode::LESS_THAN,   	  // COMMA
	AsciiCode::UNDERSCORE,        // HYPHEN
	AsciiCode::GREATER_THAN,      // PERIOD_OR_DOT
	AsciiCode::QUESTION_MARK,     // SLASH_OR_DIVIDE
	AsciiCode::CLOSE_PARENTHESIS, // ZERO
	AsciiCode::EXCLAMATION_MARK,  // ONE
	AsciiCode::AT_SYMBOL,		  // TWO
	AsciiCode::NUMBER,			  // THREE
	AsciiCode::DOLLAR,			  // FOUR
	AsciiCode::PERCENT,			  // FIVE
	AsciiCode::CARET,			  // SIX
	AsciiCode::AMPERSAND,		  // SEVEN
	AsciiCode::ASTERISK,		  // EIGHT
	AsciiCode::OPEN_PARENTHESIS,  // NINE
	AsciiCode::NUL,			      // COLON
	AsciiCode::COLON,		  	  // SEMICOLON
	AsciiCode::NUL,			  	  // LESS_THAN
	AsciiCode::PLUS,		      // EQUALS
	AsciiCode::NUL,	              // GREATER_THAN
	AsciiCode::NUL,			      // QUESTION_MARK
	AsciiCode::NUL,			      // AT_SYMBOL
	AsciiCode::NUL,	              // UPPERCASE_A
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,			   // UPPERCASE_Z
	AsciiCode::OPENING_BRACE,  // OPENING_BRACKET
	AsciiCode::VERTICAL_BAR,   // BACKSLASH
	AsciiCode::CLOSING_BRACE,  // CLOSING_BRACKET
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::TILDE_OR_EQUIVALENCY_SIGN,  // GRAVE_ACCENT
	AsciiCode::UPPERCASE_A,       // LOWERCASE_A
	AsciiCode::UPPERCASE_B,
	AsciiCode::UPPERCASE_C,
	AsciiCode::UPPERCASE_D,
	AsciiCode::UPPERCASE_E,
	AsciiCode::UPPERCASE_F,
	AsciiCode::UPPERCASE_G,
	AsciiCode::UPPERCASE_H,
	AsciiCode::UPPERCASE_I,
	AsciiCode::UPPERCASE_J,
	AsciiCode::UPPERCASE_K,
	AsciiCode::UPPERCASE_L,
	AsciiCode::UPPERCASE_M,
	AsciiCode::UPPERCASE_N,
	AsciiCode::UPPERCASE_O,
	AsciiCode::UPPERCASE_P,
	AsciiCode::UPPERCASE_Q,
	AsciiCode::UPPERCASE_R,
	AsciiCode::UPPERCASE_S,
	AsciiCode::UPPERCASE_T,
	AsciiCode::UPPERCASE_U,
	AsciiCode::UPPERCASE_V,
	AsciiCode::UPPERCASE_W,
	AsciiCode::UPPERCASE_X,
	AsciiCode::UPPERCASE_Y,
	AsciiCode::UPPERCASE_Z,      // LOWERCASE_Z
	AsciiCode::NUL,   			 // OPENING_BRACE
	AsciiCode::NUL,				 // VERTICAL_BAR
	AsciiCode::NUL,				 // CLOSING_BRACE
	AsciiCode::NUL, 		  	 // TILDE_OR_EQUIVALENCY_SIGN
	AsciiCode::NUL			  	 // DELETE
};

constexpr static size_t SHIFT_FOR_SHIFTED_BACK_ASCII_CODES = 0x21;
static AsciiCode asciiShiftedBack[128 - SHIFT_FOR_SHIFTED_BACK_ASCII_CODES] = {
	AsciiCode::ONE,  			  // EXCLAMATION_MARK
	AsciiCode::SINGLE_QUOTE,      // DOUBLE_QUOTES
	AsciiCode::THREE,  			  // NUMBER
	AsciiCode::FOUR,  			  // DOLLAR
	AsciiCode::FIVE,  			  // PERCENT
	AsciiCode::SEVEN,  			  // AMPERSAND
	AsciiCode::NUL,				  // SINGLE_QUOTE
	AsciiCode::NINE,  			  // OPEN_PARENTHESIS
	AsciiCode::ZERO,  			  // CLOSE_PARENTHESIS
	AsciiCode::EIGHT,  			  // ASTERISK
	AsciiCode::EQUALS,  		  // PLUS
	AsciiCode::NUL,				  // COMMA
	AsciiCode::NUL,        		  // HYPHEN
	AsciiCode::NUL,  	 		  // PERIOD_OR_DOT
	AsciiCode::NUL,  	  		  // SLASH_OR_DIVIDE
	AsciiCode::NUL,				  // ZERO
	AsciiCode::NUL,  		      // ONE
	AsciiCode::NUL,		          // TWO
	AsciiCode::NUL,  			  // THREE
	AsciiCode::NUL, 			  // FOUR
	AsciiCode::NUL, 			  // FIVE
	AsciiCode::NUL, 			  // SIX
	AsciiCode::NUL,     		  // SEVEN
	AsciiCode::NUL,  			  // EIGHT
	AsciiCode::NUL,		   	 	  // NINE
	AsciiCode::SEMICOLON,		  // COLON
	AsciiCode::NUL,		          // SEMICOLON
	AsciiCode::COMMA,			  // LESS_THAN
	AsciiCode::NUL,		      	  // EQUALS
	AsciiCode::PERIOD_OR_DOT,     // GREATER_THAN
	AsciiCode::SLASH_OR_DIVIDE,   // QUESTION_MARK
	AsciiCode::TWO,			      // AT_SYMBOL
	AsciiCode::LOWERCASE_A,       // UPPERCASE_A
	AsciiCode::LOWERCASE_B,
	AsciiCode::LOWERCASE_C,
	AsciiCode::LOWERCASE_D,
	AsciiCode::LOWERCASE_E,
	AsciiCode::LOWERCASE_F,
	AsciiCode::LOWERCASE_G,
	AsciiCode::LOWERCASE_H,
	AsciiCode::LOWERCASE_I,
	AsciiCode::LOWERCASE_J,
	AsciiCode::LOWERCASE_K,
	AsciiCode::LOWERCASE_L,
	AsciiCode::LOWERCASE_M,
	AsciiCode::LOWERCASE_N,
	AsciiCode::LOWERCASE_O,
	AsciiCode::LOWERCASE_P,
	AsciiCode::LOWERCASE_Q,
	AsciiCode::LOWERCASE_R,
	AsciiCode::LOWERCASE_S,
	AsciiCode::LOWERCASE_T,
	AsciiCode::LOWERCASE_U,
	AsciiCode::LOWERCASE_V,
	AsciiCode::LOWERCASE_W,
	AsciiCode::LOWERCASE_X,
	AsciiCode::LOWERCASE_Y,
	AsciiCode::LOWERCASE_Z,      // UPPERCASE_Z
	AsciiCode::NUL,              // OPENING_BRACKET
	AsciiCode::NUL,		         // BACKSLASH
	AsciiCode::NUL,		         // CLOSING_BRACKET
	AsciiCode::SIX,				 // CARET
	AsciiCode::HYPHEN,		     // UNDERSCORE
	AsciiCode::NUL,			     // GRAVE_ACCENT
	AsciiCode::NUL,      		 // LOWERCASE_A
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,
	AsciiCode::NUL,      		 // LOWERCASE_Z
	AsciiCode::OPENING_BRACKET,	 // OPENING_BRACE
	AsciiCode::SLASH_OR_DIVIDE,  // VERTICAL_BAR
	AsciiCode::CLOSING_BRACKET,  // CLOSING_BRACE
	AsciiCode::GRAVE_ACCENT,     // TILDE_OR_EQUIVALENCY_SIGN
	AsciiCode::NUL				 // DELETE
};

static UsbKey asciiCodeToUsbKey[128] = {
	UsbKey::NOT_A_KEY,         // 0x00 Null
	UsbKey::NOT_A_KEY,         // 0x01
	UsbKey::NOT_A_KEY,         // 0x02
	UsbKey::NOT_A_KEY,         // 0x03
	UsbKey::NOT_A_KEY,         // 0x04
	UsbKey::NOT_A_KEY,         // 0x05
	UsbKey::NOT_A_KEY,         // 0x06
	UsbKey::NOT_A_KEY,         // 0x07
	UsbKey::KEY_BACKSPACE,     // 0x08 Backspace
	UsbKey::KEY_TAB,           // 0x09 Horizontal Tab
	UsbKey::KEY_RETURN,        // 0x0A Line Feed
	UsbKey::NOT_A_KEY,         // 0x0B
	UsbKey::NOT_A_KEY,         // 0x0C
	UsbKey::KEY_RETURN,        // 0x0D Carriage return
	UsbKey::NOT_A_KEY,         // 0x0E
	UsbKey::NOT_A_KEY,         // 0x0F
	UsbKey::NOT_A_KEY,         // 0x10
	UsbKey::NOT_A_KEY,         // 0x11
	UsbKey::NOT_A_KEY,         // 0x12
	UsbKey::NOT_A_KEY,         // 0x13
	UsbKey::NOT_A_KEY,         // 0x14
	UsbKey::NOT_A_KEY,         // 0x15
	UsbKey::NOT_A_KEY,         // 0x16
	UsbKey::NOT_A_KEY,         // 0x17
	UsbKey::NOT_A_KEY,         // 0x18
	UsbKey::NOT_A_KEY,         // 0x19
	UsbKey::NOT_A_KEY,         // 0x1A
	UsbKey::KEY_ESCAPE,        // 0x1B Escape
	UsbKey::NOT_A_KEY,         // 0x1C
	UsbKey::NOT_A_KEY,         // 0x1D
	UsbKey::NOT_A_KEY,         // 0x1E
	UsbKey::NOT_A_KEY,         // 0x1F
	UsbKey::KEY_SPACEBAR,         				// 0x20
	UsbKey::KEY_1_EXCLAMATION_MARK,             // 0x21 !
	UsbKey::KEY_2_AT,    						// 0x22 "
	UsbKey::KEY_3_NUMBER_SIGN,             		// 0x23 #
	UsbKey::KEY_4_DOLLAR,             			// 0x24 $
	UsbKey::KEY_5_PERCENT,             			// 0x25 %
	UsbKey::KEY_7_AMPERSAND,             		// 0x26 &
	UsbKey::KEY_SINGLE_AND_DOUBLE_QUOTE,    	// 0x27 '
	UsbKey::KEY_9_OPARENTHESIS,             	// 0x28 (
	UsbKey::KEY_0_CPARENTHESIS,             	// 0x29 )
	UsbKey::KEY_8_ASTERISK,             		// 0x2A *
	UsbKey::KEY_EQUAL_PLUS,         			// 0x2B +
	UsbKey::KEY_COMMA_AND_LESS,         		// 0x2C ,
	UsbKey::KEY_MINUS_UNDERSCORE,        		// 0x2D -
	UsbKey::KEY_DOT_GREATER,        			// 0x2E .
	UsbKey::KEY_SLASH_QUESTION,         		// 0x2F /
	UsbKey::KEY_0_CPARENTHESIS,         		// 0x30 0
	UsbKey::KEY_1_EXCLAMATION_MARK,             // 0x31 1
	UsbKey::KEY_2_AT,            				// 0x32 2
	UsbKey::KEY_3_NUMBER_SIGN,                  // 0x33 3
	UsbKey::KEY_4_DOLLAR,                       // 0x34 4
	UsbKey::KEY_5_PERCENT,                      // 0x35 5
	UsbKey::KEY_6_CARET,                        // 0x36 6
	UsbKey::KEY_7_AMPERSAND,              		// 0x37 7
	UsbKey::KEY_8_ASTERISK,             		// 0x38 8
	UsbKey::KEY_9_OPARENTHESIS,             	// 0x39 9
	UsbKey::KEY_SEMICOLON_COLON,     			// 0x3A :
	UsbKey::KEY_SEMICOLON_COLON,     			// 0x3B ;
	UsbKey::KEY_COMMA_AND_LESS,         		// 0x3C <
	UsbKey::KEY_EQUAL_PLUS,         			// 0x3D =
	UsbKey::KEY_DOT_GREATER,        			// 0x3E >
	UsbKey::KEY_SLASH_QUESTION,         		// 0x3F ?
	UsbKey::KEY_2_AT,          // 0x40 @
	UsbKey::KEY_A,             // 0x41 A
	UsbKey::KEY_B,             // 0x42 B
	UsbKey::KEY_C,             // 0x43 C
	UsbKey::KEY_D,             // 0x44 D
	UsbKey::KEY_E,             // 0x45 E
	UsbKey::KEY_F,             // 0x46 F
	UsbKey::KEY_G,             // 0x47 G
	UsbKey::KEY_H,             // 0x48 H
	UsbKey::KEY_I,             // 0x49 I
	UsbKey::KEY_J,             // 0x4A J
	UsbKey::KEY_K,             // 0x4B K
	UsbKey::KEY_L,             // 0x4C L
	UsbKey::KEY_M,             // 0x4D M
	UsbKey::KEY_N,             // 0x4E N
	UsbKey::KEY_O,             // 0x4F O
	UsbKey::KEY_P,             // 0x50 P
	UsbKey::KEY_Q,             // 0x51 Q
	UsbKey::KEY_R,             // 0x52 R
	UsbKey::KEY_S,             // 0x53 S
	UsbKey::KEY_T,             // 0x55 T
	UsbKey::KEY_U,             // 0x55 U
	UsbKey::KEY_V,             // 0x56 V
	UsbKey::KEY_W,             // 0x57 W
	UsbKey::KEY_X,             // 0x58 X
	UsbKey::KEY_Y,             // 0x59 Y
	UsbKey::KEY_Z,             // 0x5A Z
	UsbKey::KEY_OBRACKET_AND_OBRACE,  		// 0x5B [
	UsbKey::KEY_BACKSLASH_VERTICAL_BAR,     // 0x5C '\'
	UsbKey::KEY_CBRACKET_AND_CBRACE, 		// 0x5D ]
	UsbKey::KEY_6_CARET,             		// 0x5E ^
	UsbKey::KEY_MINUS_UNDERSCORE,         	// 0x5F _
	UsbKey::KEY_GRAVE_ACCENT_AND_TILDE,     // 0x60 `
	UsbKey::KEY_A,             // 0x61 a
	UsbKey::KEY_B,             // 0x62 b
	UsbKey::KEY_C,             // 0x63 c
	UsbKey::KEY_D,             // 0x66 d
	UsbKey::KEY_E,             // 0x65 e
	UsbKey::KEY_F,             // 0x66 f
	UsbKey::KEY_G,             // 0x67 g
	UsbKey::KEY_H,             // 0x68 h
	UsbKey::KEY_I,             // 0x69 i
	UsbKey::KEY_J,             // 0x6A j
	UsbKey::KEY_K,             // 0x6B k
	UsbKey::KEY_L,             // 0x6C l
	UsbKey::KEY_M,             // 0x6D m
	UsbKey::KEY_N,             // 0x6E n
	UsbKey::KEY_O,             // 0x6F o
	UsbKey::KEY_P,             // 0x70 p
	UsbKey::KEY_Q,             // 0x71 q
	UsbKey::KEY_R,             // 0x72 r
	UsbKey::KEY_S,             // 0x73 s
	UsbKey::KEY_T,             // 0x75 t
	UsbKey::KEY_U,             // 0x75 u
	UsbKey::KEY_V,             // 0x76 v
	UsbKey::KEY_W,             // 0x77 w
	UsbKey::KEY_X,             // 0x78 x
	UsbKey::KEY_Y,             // 0x79 y
	UsbKey::KEY_Z,             // 0x7A z
	UsbKey::KEY_OBRACKET_AND_OBRACE,  		// 0x7B {
	UsbKey::KEY_BACKSLASH_VERTICAL_BAR,     // 0x7C |
	UsbKey::KEY_CBRACKET_AND_CBRACE, 		// 0x7D }
	UsbKey::KEY_GRAVE_ACCENT_AND_TILDE,     // 0x7E ~
	UsbKey::KEY_DELETE         				// 0x7F Delete
};

}

namespace Keys {

AsciiCode getAsciiByUsbKey(UsbKey code)
{
	return usbKeyToAsciiCode[static_cast<AsciiCodeType>(code)];
}

UsbKey getUsbKeyByAscii(AsciiCode code)
{
	return asciiCodeToUsbKey[static_cast<AsciiCodeType>(code)];
}

AsciiCode getAsciiShifted(AsciiCode code)
{
	return asciiShifted[static_cast<AsciiCodeType>(code) - SHIFT_FOR_SHIFTED_ASCII_CODES];
}

AsciiCode getAsciiBackShifted(AsciiCode code)
{
	return asciiShiftedBack[static_cast<AsciiCodeType>(code) - SHIFT_FOR_SHIFTED_BACK_ASCII_CODES];
}

bool isAsciiShifted(AsciiCode code)
{
	return (getAsciiBackShifted(code) != AsciiCode::NUL);
}

Key::Key():
	_usbKey(UsbKey::NOT_A_KEY),
	_asciiCode(AsciiCode::NUL),
	_keyType(KeyType::SYBMOL)
{ }

Key::Key(UsbKeyCodeType key, UsbKeyCodeType modifier)
{
	set(key, modifier);
}

Key::Key(UsbKey key, UsbSpecialKeySequence modifier)
{
	set(key, modifier);
}

Key::Key(AsciiCodeType code)
{
	set(code);
}

Key::Key(AsciiCode code)
{
	set(static_cast<AsciiCodeType>(code));
}

Key::Key(UsbKeyCodeType key, UsbSpecialKey modifier)
{
	set(key, static_cast<UsbKeyCodeType>(modifier));
}

Key::Key(UsbKey key, UsbKeyCodeType modifier)
{
	set(static_cast<UsbKeyCodeType>(key), modifier);
}

Key::Key(UsbKey key, UsbSpecialKey modifier)
{
	set(key, modifier);
}

Key::Key(UsbKey key)
{
	set(key, UsbSpecialKey::NO_KEY);
}

Key::~Key()
{ }

void Key::set(AsciiCodeType code)
{
	_asciiCode = static_cast<AsciiCode>(code);
	_usbKey = getUsbKeyByAscii(_asciiCode);
	_usbKeyModifier.clear();

	if (isAsciiShifted(_asciiCode)) {
		_usbKeyModifier = UsbSpecialKey::LEFT_SHIFT;
		_usbKey = getUsbKeyByAscii(
				getAsciiBackShifted(_asciiCode)
			);
	}

	_updateKeyType();
}

void Key::set(UsbKey key, UsbSpecialKey modifier)
{
	set(
		static_cast<UsbKeyCodeType>(key),
		static_cast<UsbKeyCodeType>(modifier)
	);
}

void Key::set(UsbKeyCodeType key, UsbKeyCodeType modifier)
{
	_usbKey = static_cast<UsbKey>(key);
	_usbKeyModifier = modifier;

	_asciiCode = getAsciiByUsbKey(_usbKey);

	_checkAsciiShifted();

	_updateKeyType();
}

void Key::set(UsbKey key, UsbSpecialKeySequence modifier)
{
	_usbKey = key;
	_usbKeyModifier = modifier;

	_asciiCode = getAsciiByUsbKey(_usbKey);

	_checkAsciiShifted();

	_updateKeyType();
}

void Key::_checkAsciiShifted()
{
	if (_asciiCode != AsciiCode::NUL) {
		bool usbModifierIsShift =
				(_usbKeyModifier == UsbSpecialKey::LEFT_SHIFT ||
				 _usbKeyModifier == UsbSpecialKey::RIGHT_SHIFT);

		if (usbModifierIsShift) {
			if (!isAsciiShifted(_asciiCode)) {
				_asciiCode = getAsciiShifted(_asciiCode);
			}
		}
		else {
			if (isAsciiShifted(_asciiCode)) {
				_asciiCode = getAsciiBackShifted(_asciiCode);
			}
		}
	}
}

void Key::_updateKeyType()
{
	if (_usbKey == UsbKey::KEY_RIGHTARROW ||
		_usbKey == UsbKey::KEY_LEFTARROW  ||
		_usbKey == UsbKey::KEY_UPARROW    ||
		_usbKey == UsbKey::KEY_DOWNARROW  ||
		_usbKey == UsbKey::KEY_ENTER	  ||
		_usbKey == UsbKey::KEY_ESCAPE)
	{
		_keyType = CONTROL;
	}
	else {
		_keyType = SYBMOL;
	}
}

} /* namespace Keys */
