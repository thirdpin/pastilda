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

#ifndef PACKGES_PACKAGEFACTORY_H_
#define PACKGES_PACKAGEFACTORY_H_

#include <cstddef>
#include <string>
#include <experimental/string_view>

#include <usb_deque.h>
#include <keys/Key.h>

using std::experimental::basic_string_view;
using namespace Keys;
using std::size_t;
using std::string;

namespace UsbPackages {

class PackageFactory {
public:
	typedef const AsciiCodeType* InputDataConst;
	typedef UsbRawData* OutputData;
	using PackageDeque = UsbDequeStandart;

	PackageFactory();
	PackageFactory(PackageDeque* deque);
	~PackageFactory();

	void processData(InputDataConst inputData, size_t inputDataLength);
	void processData(const basic_string_view<uint8_t>& inputData);
	void processData(const char* inputData, size_t inputDataLength);
	void generateClearSequence();
	void generateClearSequence(size_t count);
	void generateEmptyPackage();
	void generateTabPackage();
	void generateEnterPackage();
	void generatePackage(InputDataConst inputData);

private:
	constexpr static UsbRawData _EMPTY_FIELD = 0;

	PackageDeque* _packageDeque;

	InputDataConst _inputData;
	size_t _inputDataLength;

	UsbPackage* _currentPackage;
	size_t _currentPackageFieldNum;

	Key _lastKey;

	void _processInputData();

	void _addEmptyPackage();
	void _addClearPackage();
	void _addShiftedPackage(Key& key);

	void _addSimplePackage(Key& key);
	void _continueSimplePackage(Key& key);
	void _newSimplePackage(Key& key);
	void _checkSimplePackageEnd();
	void _completeLastSimplePackage();
};

} /* namespace UsbPackages */

#endif /* PACKGES_PACKAGEFACTORY_H_ */
