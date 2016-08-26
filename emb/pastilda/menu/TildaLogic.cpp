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

#include <cstring>
#include <cstdio>

#include <FastDelegate.h>

#include "systick_ext.h"
#include <keys/Key.h>

#include <TildaLogic.h>

using std::string;
using std::size_t;

using namespace Keys;
using namespace DB;

namespace fd = fastdelegate;

namespace Logic {
using namespace Logic::Private;

TildaLogic::TildaLogic(UsbDequeStandart* deque,
					   const SpecialPoints& specialPoints):
	_inputData(nullptr),
	_inputDataLength(0),
	_inputPackagePtr(&ZERO_PACKAGE),
	_packageFactory(PackageFactory(deque)),
	_db(nullptr),
	_fixedMenuCbs(new FixedMenuCallbacks(this)),
	_lastPackage(ZERO_PACKAGE),
	_lastKeysBufferLen(0),
	_keyboardInput(KeyboardLikeInput<KeyBuffer>(&_keysBuffer
#ifdef DEBUG
			, &_packageFactory
#endif
	))
{
	_specialMenuPoints.formatFat = specialPoints.formatFat;

	_init();
}

TildaLogic::~TildaLogic()
{ }

void TildaLogic::_init()
{
	_fixedMenu.settings = {
			Strings::SETTINGS_POINT,
			std::strlen(Strings::SETTINGS_POINT),
			MenuTreeT::TreeNodeT::EMPTY_CALLBACK
	};

	_fixedMenu.exit = {
			Strings::EXIT_POINT,
			strlen(Strings::EXIT_POINT),
			fd::MakeDelegate(_fixedMenuCbs, &FixedMenuCallbacks::exit)
	};

	_tildaKey = TILDA_MODE_KEY;
	_tildaSeq.set(UsbSpecialKey::LEFT_CTRL);

	_setState(State::PASSIVE_MODE);
}

void TildaLogic::_buildMenu()
{
	_db.init(_keepassReader.get_xml());

	_menuTree.destroy();
	_buildMenuTree();

	_menu.init(&_menuTree);
	_menu.moveTop();
}

void TildaLogic::_buildMenuTree()
{
	constexpr uint32_t startIndex = 0;
	_buildTree(startIndex);

	_menuTree.moveInHead();
	_menuTree.moveMostRight();
	_buildFixedMenu();
}

void TildaLogic::_buildTree(uint32_t index)
{
	_copyDbContainer(index++);

	if (_db.isMostBottom() == false) {
		_db.moveInto();

		_menuTree.addChild();
		_menuTree.moveInto();
		_menuTree.moveMostRight();

		_buildTree(index++);
	}
	else {
		while (_db.isMostRight() == false) {
			_db.moveRight();

			_menuTree.moveOut();
			if (_db.isMostTop()) {
				_menuTree.addNeighbor();
				_menuTree.moveRight();
			}
			else {
				_menuTree.addChild();
				_menuTree.moveInto();
				_menuTree.moveMostRight();
			}

			_buildTree(index++);
		}

		_db.moveOut();
		_menuTree.moveOut();
	}
}

void TildaLogic::_buildFixedMenu()
{
	_menuTree.addNeighbor();
	_menuTree.moveRight();
	_addMenuPoint(_fixedMenu.settings);

	// Add special menu points
	_menuTree.addChild();
	_menuTree.moveInto();
	_addMenuPoint(_specialMenuPoints.formatFat);
	_menuTree.getCurrentNode()->setCallback(
			fd::MakeDelegate(_fixedMenuCbs, &FixedMenuCallbacks::formatFat)
		);
	// Another points
	_menuTree.moveOut();

	_menuTree.addNeighbor();
	_menuTree.moveRight();
	_addMenuPoint(_fixedMenu.exit);
	_menuTree.getCurrentNode()->setCallback(
			_fixedMenu.exit.callback
		);
}

void TildaLogic::_copyDbContainer(uint32_t index)
{
	MenuTreeT::TreeNodePtr currentNode = _menuTree.getCurrentNode();
	MenuTreeT::ContainerT& currentContainer = currentNode->getContainer();

	const XmlTree::NodeStruct& srcNode = _db.getCurrentNodeInfo();

	currentContainer.setIndex(index);
	currentContainer.setName(srcNode.name.data(), srcNode.name.length());
	currentContainer.setLogin(srcNode.login.data(), srcNode.login.length());
	currentContainer.setPassword(srcNode.password.data(),
								 srcNode.password.length());
	currentContainer.setType(srcNode.type.data(), srcNode.type.length());

	if (_db.isEndNode()) {
		currentNode->setCallback(
			fd::MakeDelegate(
					this,
					&TildaLogic::_logInCallback
				)
			);
	}
}

template<typename T>
void TildaLogic::_addMenuPoint(T& point)
{
	MenuTreeT::TreeNodePtr node = _menuTree.getCurrentNode();
	node->getContainer().setName(
		reinterpret_cast<const StringFieldChar*>(
				point.name
			),
			point.nameLength
	);
}

void TildaLogic::_logInCallback(MenuT::ContainerT& container)
{
	_packageFactory.generateClearSequence(container.getName().length());

	// Login enter
	_packageFactory.processData(container.getLogin());

	// Switch field
	StringFieldConst& type = container.getType();
	if (type == Strings::FORM) {
		_packageFactory.generateTabPackage();
	}
	else {
		_packageFactory.generateEnterPackage();
		delay_ms(800);  // Wait console reaction
	}

	// Password enter
	_packageFactory.processData(container.getPassword());

	// Press "Enter"
	_packageFactory.generateEnterPackage();

	_setState(State::MENU_MODE_END);
}

void TildaLogic::process(DataBufferConst inputData, size_t inputDataLength)
{
	_inputData = inputData;
	_inputDataLength = inputDataLength;
	_inputPackagePtr = reinterpret_cast<UsbPackageConst*>(inputData);

	switch (_currentState)
	{
		case State::PASSIVE_MODE:
			_checkMode();
			_redirectInput();
			_lastPackage = *_inputPackagePtr;
		break;

		case State::ENTER_MASTER_PASSWORD:
			_processMasterPassword();
		break;

		case State::MENU_MODE_START:
			_sendMenuGreeting();
			_setState(State::MENU_MODE);
		break;

		case State::MENU_MODE:
			_processMenuMode();
			_lastPackage = *_inputPackagePtr;
		break;

		default:
		break;
	}
}

void TildaLogic::_processMasterPassword()
{
	if (_inputPackagePtr->key[0] == UsbKey::KEY_ENTER) {
		const char* passw = (const char*)_keysBuffer.data();
		size_t passwLen = _keysBuffer.size();

		_dbDecrypt(passw, passwLen);

		_keysBuffer.resize(0);
		_packageFactory.generateClearSequence(_lastKeysBufferLen);
		_lastKeysBufferLen = 0;

		if (_currentState == State::MASTER_PASSWORD_PASSED) {
			_buildMenu();

			_setState(State::MENU_MODE_START);
		}
		else {
			_clearMsg(Strings::GREETING_TO_WRITE);

			_sendMsg(_getDbErrorType());
			delay_ms(WRONG_PASSWORD_DELAY);
			_clearMsg(_getDbErrorType());

			_setState(State::ENTER_MASTER_PASSWORD);

			_sendMsg(Strings::GREETING_TO_WRITE);
		}
	}
	else if (_inputPackagePtr->key[0] == UsbKey::KEY_ESCAPE) {
		_packageFactory.generateClearSequence(_lastKeysBufferLen);
		_lastKeysBufferLen = 0;

		_clearMsg(Strings::GREETING_TO_WRITE);
		_setState(State::PASSIVE_MODE);
	}
	else {
		_keyboardInput.process(_inputPackagePtr);

		int32_t newOutputLen =
				(int32_t)_keysBuffer.size() - (int32_t)_lastKeysBufferLen;
		if (newOutputLen > 0) {
#ifdef DEBUG
			_packageFactory.processData(" >", 2);
			for (size_t i = _lastKeysBufferLen; i < _keysBuffer.size(); ++i) {
				_packageFactory.processData((char*)&_keysBuffer[i], 1);
			}
#else
			for (size_t i = 0; i < newOutputLen; ++i) {
				_sendMsg(Strings::PASSWORD_SYMB);
			}
#endif
		}
		else {
			_packageFactory.generateClearSequence(abs(newOutputLen));
			_packageFactory.generateEmptyPackage();
		}

		_lastKeysBufferLen = _keysBuffer.size();
	}
}

void TildaLogic::_dbDecrypt(const char* passwd, size_t len)
{
	using namespace KeepAss;

	_keepassReader.set_password(passwd, len);

	_dbState = _keepassReader.decrypt_database(KEEPASS_BASE_FILE);

	if (_dbState == DecryptionResult::SUCCESS) {
		_setState(State::MASTER_PASSWORD_PASSED);
	}
	else {
		_setState(State::PASSIVE_MODE);
	}
}

void TildaLogic::_redirectInput()
{
	_packageFactory.generatePackage(_inputData);
}

inline void TildaLogic::_checkMode()
{
	if (_inputPackagePtr->key[0] == UsbKey::NOT_A_KEY) {
		if (_tildaKey == _lastPackage.key[0] &&
			_tildaSeq == _lastPackage.special)
		{
			_switchMenuMode();
			_lastPackage = ZERO_PACKAGE;

			if (_currentState == State::MENU_MODE_START) {
				_sendMsg(Strings::GREETING_TO_WRITE);

				_setState(State::ENTER_MASTER_PASSWORD);
			}
		}
	}
}

inline void TildaLogic::_sendMenuGreeting()
{
	_menu.moveTop();

	_clearMsg(Strings::GREETING_TO_WRITE);
	_packageFactory.processData(
		_menu.getCurrentPointContainer().getName()
	);
}

inline void TildaLogic::_switchMenuMode()
{
	_keysBuffer.resize(0);
	if (_currentState == State::MENU_MODE ||
		_currentState == State::MENU_MODE_END ||
		_currentState == State::ENTER_MASTER_PASSWORD)
	{
		_setState(State::PASSIVE_MODE);
	}
	else {
		_setState(State::MENU_MODE_START);
	}
}

void TildaLogic::_processMenuMode()
{
	Key key {
		_inputPackagePtr->key[0],
		_inputPackagePtr->special
	};

	if (key == UsbKey::KEY_ESCAPE) {
		_keysBuffer.resize(0);

		StringFieldConst currentPoint =
						_menu.getCurrentPointContainer().getName();
		_packageFactory.generateClearSequence(currentPoint.length());
		_setState(State::PASSIVE_MODE);
	}
	else if (key.isControl()) {
		_keysBuffer.resize(0);  // reset key buffer

		StringFieldConst currentPoint =
				_menu.getCurrentPointContainer().getName();

		_menu.processKey(key);

		StringFieldConst& newPoint =
				_menu.getCurrentPointContainer().getName();

		if (_currentState == State::MENU_MODE_END) {
			_switchMenuMode();
			_setState(State::PASSIVE_MODE);
		}
		else if (newPoint.compare(currentPoint) != 0) {
			_packageFactory.generateClearSequence(currentPoint.length());
			_packageFactory.processData(newPoint);
		}
	}
	// TODO: place it into separate class, into Menu (probably)
//	else if (key != UsbKey::NOT_A_KEY) { // in search mode
//		_setState(State::SEARCH_MODE);
//	}
}

void TildaLogic::_searchMode()
{
	StringFieldConst currentPoint =
			_menu.getCurrentPointContainer().getName();

	size_t pos = _menu.search(
			StringField(_keysBuffer.data(), _keysBuffer.size())
		);

	StringFieldConst& newPoint =
			_menu.getCurrentPointContainer().getName();

	bool pointChanged = newPoint.compare(currentPoint);

	if (pos != StringField::npos && pointChanged) {
		_packageFactory.generateClearSequence(currentPoint.length());
		_packageFactory.processData(newPoint);
	}
}

const char* TildaLogic::_getDbErrorType()
{
	using namespace KeepAss;

	switch (_dbState) {
		case DecryptionResult::CREDENTIALS_ERROR:
			return Strings::CREDENTIALS_ERROR;
		break;

		case DecryptionResult::DATA_HASH_ERROR:
			return Strings::DATA_HASH_ERROR;
		break;

		case DecryptionResult::DB_FILE_ERROR:
			return Strings::DB_FILE_ERROR;
		break;

		case DecryptionResult::MASTER_KEY_ERROR:
			return Strings::MASTER_KEY_ERROR;
		break;

		case DecryptionResult::SIGNATURE_ERROR:
			return Strings::SIGNATURE_ERROR;
		break;

		default:
			return Strings::PASSWORD_WRONG;
		break;
	}
}

void TildaLogic::_sendMsg(const char* msg)
{
	_packageFactory.processData(msg, std::strlen(msg));
}

void TildaLogic::_clearMsg(const char* msg)
{
	_packageFactory.generateClearSequence(strlen(msg));
}

namespace Private {
	FixedMenuCallbacks::FixedMenuCallbacks(TildaLogic* logic) :
		_logic(logic)
	{ }

	void FixedMenuCallbacks::formatFat(TildaLogic::CallbackArg arg)
	{
		_logic->_specialMenuPoints.formatFat.callback();

		_logic->_setState(TildaLogic::State::MENU_MODE_END);
	}

	void FixedMenuCallbacks::exit(TildaLogic::CallbackArg arg)
	{
		_logic->_setState(TildaLogic::State::MENU_MODE_END);
	}
}

}  // namespace Logic
