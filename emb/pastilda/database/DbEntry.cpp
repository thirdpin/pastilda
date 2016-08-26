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

#include <DbEntry.h>

namespace DB {

Entry::Entry() :
		_index(0),
		_name(EMPTY_FIELD),
		_login(EMPTY_FIELD),
		_password(EMPTY_FIELD),
		_type(EMPTY_FIELD)
{ }

Entry::Entry(size_t index) :
		_index(index),
		_name(EMPTY_FIELD),
		_login(EMPTY_FIELD),
		_password(EMPTY_FIELD),
		_type(EMPTY_FIELD)
{ }

Entry::~Entry()
{ }

Entry::Entry(const Entry& entry)
{
	_index = entry.getIndex();
	_name = entry.getName();
	_login = entry.getLogin();
	_password = entry.getPassword();
	_type = entry.getType();
}

void Entry::setIndex(uint32_t index)
{
	_index = index;
}

void Entry::setName(const uint8_t* name, size_t length)
{
	StringField nameString(name, length);
	_name.swap(nameString);
}

void Entry::setLogin(const uint8_t* login, size_t length)
{
	StringField loginString(login, length);
	_login.swap(loginString);
}

void Entry::setPassword(const uint8_t* password, size_t length)
{
	StringField passwordString(password, length);
	_password.swap(passwordString);
}

void Entry::setType(const uint8_t* type, size_t length)
{
	StringField typeString(type, length);
	_type.swap(typeString);
}

}
