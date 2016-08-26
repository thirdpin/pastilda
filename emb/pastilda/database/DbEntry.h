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

#ifndef DATABASE_DBENTRY_H_
#define DATABASE_DBENTRY_H_

#include <experimental/string_view>
#include <cstddef>
#include <cstdint>

using std::size_t;
using std::experimental::basic_string_view;

namespace DB {

using StringField = basic_string_view<uint8_t>;
using StringFieldChar = StringField::value_type;
using StringFieldConst = const StringField;

constexpr static const StringFieldChar* EMPTY_FIELD_CHAR =
		reinterpret_cast<const StringFieldChar*>("");

constexpr static StringField EMPTY_FIELD(EMPTY_FIELD_CHAR, 1);

class Entry
{
public:
	Entry();
	Entry(size_t index);
	~Entry();

	Entry(const Entry& entry);

	void setIndex(uint32_t index);
	void setName(const uint8_t* name, size_t length);
	void setLogin(const uint8_t* login, size_t length);
	void setPassword(const uint8_t* password, size_t length);
	void setType(const uint8_t* type, size_t length);

	uint32_t getIndex() const {
		return _index;
	}

	StringFieldConst& getLogin() const {
		return _login;
	}

	StringFieldConst& getName() const {
		return _name;
	}

	StringFieldConst& getPassword() const {
		return _password;
	}

	StringFieldConst& getType() const {
		return _type;
	}

private:
	uint32_t _index;
	StringField _name;
	StringField _login;
	StringField _password;

	StringField _type;
};

} // namespace DB


#endif /* DATABASE_DBENTRY_H_ */
