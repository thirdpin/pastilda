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

#include <cstddef>
#include <cstring>

#include <keys/Key.h>

#include <usb_deque.h>
#include <UsbPackageFactory.h>

using std::size_t;
using namespace Keys;

namespace UsbPackages {

PackageFactory::PackageFactory() :
	_packageDeque(new PackageDeque()),
	_inputData(nullptr),
	_inputDataLength(0),
	_currentPackage(nullptr),
	_currentPackageFieldNum(0),
	_lastKey(UsbKey::NOT_A_KEY)
{ }

PackageFactory::PackageFactory(PackageDeque* deque) :
	_packageDeque(deque),
	_inputData(nullptr),
	_inputDataLength(0),
	_currentPackage(nullptr),
	_currentPackageFieldNum(0),
	_lastKey(UsbKey::NOT_A_KEY)
{ }


PackageFactory::~PackageFactory()
{ }

void PackageFactory::generatePackage(InputDataConst inputData)
{
	_packageDeque->unlock();
		auto inputPackage = reinterpret_cast<UsbPackageConst*>(inputData);
		_packageDeque->push_back(*inputPackage);
	_packageDeque->lock();
}

void PackageFactory::generateClearSequence()
{
	_addEmptyPackage();
	_addClearPackage();
	_addEmptyPackage();
}

void PackageFactory::generateClearSequence(size_t count)
{
	Key backspaceKey(UsbKey::KEY_BACKSPACE);

	_addEmptyPackage();
	for (size_t i = 0; i < count; ++i) {
		_addSimplePackage(backspaceKey);
	}
	_addEmptyPackage();
}

void PackageFactory::generateTabPackage()
{
	_addEmptyPackage();

	Key tabKey(UsbKey::KEY_TAB);
	_addSimplePackage(tabKey);

	_addEmptyPackage();
}

void PackageFactory::generateEnterPackage()
{
	_addEmptyPackage();

	Key enterKey(UsbKey::KEY_ENTER);
	_addSimplePackage(enterKey);

	_addEmptyPackage();
}

void PackageFactory::generateEmptyPackage()
{
	_addEmptyPackage();
}

void PackageFactory::_addClearPackage()
{
	Key aKey(UsbKey::KEY_A, UsbSpecialKey::LEFT_CTRL);
	Key deleteKey(UsbKey::KEY_DELETE);

	_addEmptyPackage();
	_addSimplePackage(aKey);
	_addEmptyPackage();
	_addSimplePackage(deleteKey);
	_addEmptyPackage();
}

void PackageFactory::processData(const basic_string_view<uint8_t>& inputData)
{
	_inputDataLength = inputData.length();
	_inputData = reinterpret_cast<InputDataConst>(inputData.data());
	_processInputData();
}

void PackageFactory::processData(InputDataConst inputData,
		                         size_t inputDataLength)
{
	_inputDataLength = inputDataLength;
	_inputData = inputData;
	_processInputData();
}

void PackageFactory::processData(const char* inputData,
		                         size_t inputDataLength)
{
	_inputDataLength = inputDataLength;
	_inputData = reinterpret_cast<InputDataConst>(inputData);
	_processInputData();
}

void PackageFactory::_processInputData()
{
	for (size_t iSymbol = 0; iSymbol < _inputDataLength; iSymbol++) {
		Key key(_inputData[iSymbol]);

		if (key.isShifted()) {
			_addShiftedPackage(key);
		}
		else {
			_addSimplePackage(key);
		}
	}
	_addEmptyPackage();
}

inline void PackageFactory::_completeLastSimplePackage()
{
	bool lastPackageNotCompleted = (_currentPackageFieldNum > 0);
	if (lastPackageNotCompleted) {
		_currentPackageFieldNum = 0;
		_packageDeque->lock();
	}
}

void PackageFactory::_addEmptyPackage()
{
	_completeLastSimplePackage();

	bool dequeWasLocked = _packageDeque->isLocked();
	_packageDeque->unlock();
	{
		_packageDeque->push_back();

		UsbPackage& currentPackage = _packageDeque->back();

		currentPackage.clear();
	}
	if (dequeWasLocked) {       // if deque was locked before adding empty package,
		_packageDeque->lock();  // then lock it, otherwise it's something
	}							// need deque unlocked (e.g. simple package)
}

void PackageFactory::_addShiftedPackage(Key& key)
{
	_packageDeque->unlock();
	{
		_addEmptyPackage();  // clear last key
		_addEmptyPackage();  // add new empty package

		UsbPackage& currentPackage = _packageDeque->back();

		currentPackage.special = key.getUsbKeyModifier();
		currentPackage.key[0] = key.getUsbKey();
	}
	_packageDeque->lock();
}

void PackageFactory::_addSimplePackage(Key& key)
{
	// If in one package will be two same keys one
	// by one, then the only one will be send. So we
	// should split package if we have this situation.
	if (_currentPackageFieldNum == 0 || key == _lastKey) {
		_newSimplePackage(key);
	}
	else {
		_continueSimplePackage(key);
	}

	_lastKey = key;
	_checkSimplePackageEnd();
}

void PackageFactory::_continueSimplePackage(Key& key)
{
	_currentPackageFieldNum++;
	size_t currentKeyNum =
			_currentPackageFieldNum - USB_PACKAGE_SPECIAL_FIELDS_LENGTH;

	UsbPackage& currentPackage = _packageDeque->back();
	currentPackage.key[currentKeyNum] = key.getUsbKey();
}

void PackageFactory::_newSimplePackage(Key& key)
{
	_packageDeque->unlock();

	_addEmptyPackage();  // clear last key
	_addEmptyPackage();  // add new empty package

	UsbPackage& currentPackage = _packageDeque->back();
	currentPackage.special = key.getUsbKeyModifier();
	currentPackage.reserved = _EMPTY_FIELD;
	currentPackage.key[0] = key.getUsbKey();

	_currentPackageFieldNum = 2;  // last set field
}

void PackageFactory::_checkSimplePackageEnd()
{
	if (_currentPackageFieldNum == USB_PACKAGE_LENGTH - 6) {  // USB_PACKAGE_LENGTH - 5 = 2,
		_completeLastSimplePackage();						  // what means, that there will be
	}														  // only one key per package,
															  // otherwise linux don't work
}

} /* namespace UsbPackages */
