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

#ifndef TILDALOGIC_H_
#define TILDALOGIC_H_

#include <cstring>
#include <etl/vector.h>

#include <usb_deque.h>
#include <keys/Key.h>
#include <keepass/keepass_reader.h>
#include <database/DbEntry.h>
#include <database/xmltree/XmlTree.h>
#include <KeyboardLikeInput.hpp>
#include <Menu.hpp>
#include <UsbPackageFactory.h>

using std::size_t;
using std::string;

using namespace Keys;
using namespace UsbPackages;

namespace Logic {

namespace Private {
	class FixedMenuCallbacks;

	namespace Strings {
		// Messages
		static constexpr const char* PASSWORD_WRONG = "Failed!\0";
		static constexpr const char* GREETING_TO_WRITE = "Pass:\0";
		static constexpr const char* PASSWORD_SYMB = "*\0";
		// Menu point's names
		static constexpr const char* SETTINGS_POINT = "Settings\0";
		static constexpr const char* EXIT_POINT = "Exit\0";
		static constexpr const char* FORMAT_FLASH_POINT = "Format flash\0";
		// Login/password types
		static constexpr const uint8_t* FORM = (const uint8_t*)"FORM\0";
		static constexpr const uint8_t* CONSOLE = (const uint8_t*)"CONSOLE\0";
		// Database states
		static constexpr const char* SUCCESS = "Success!\0";
		static constexpr const char* SIGNATURE_ERROR = "Signature error!\0";
		static constexpr const char* MASTER_KEY_ERROR = "Master key error!\0";
		static constexpr const char* CREDENTIALS_ERROR = "Credentials error!\0";
		static constexpr const char* DB_FILE_ERROR = "Db file error!\0";
		static constexpr const char* DATA_HASH_ERROR = "Data hash error!\0";
	};

	static constexpr size_t KEYS_BUFFER_SIZE = 128;
	static size_t WRONG_PASSWORD_DELAY = 1000;

	inline size_t abs(int32_t val) {
		return (val < 0) ? -val : val;
	}
}

class TildaLogic {
public:
	typedef UsbRawData DataT;
	typedef DataT* DataPtr;
	typedef const DataT* DataConstPtr;

	typedef DataT* DataBuffer;
	typedef const DataT* DataBufferConst;

	constexpr static size_t MAX_MENU_POINTS_COUNT = 100;
	using MenuT = Menu<MAX_MENU_POINTS_COUNT>;

	constexpr static size_t MAX_TREE_NODES_COUNT = MenuT::COUNT_OF_POINTS;
	using MenuTreeT = Tree<MAX_TREE_NODES_COUNT, MenuT::ContainerT>;

	using CallbackT = MenuT::TreeT::TreeNodeT::CallbackT;
	using CallbackArg = MenuT::TreeT::TreeNodeT::CallbackArg;

	using KeyBuffer = etl::vector<AsciiCodeType, Private::KEYS_BUFFER_SIZE>;

	enum class State {
		PASSIVE_MODE,
		MENU_MODE_START,
		MENU_MODE,
		MENU_MODE_END,
		ENTER_MASTER_PASSWORD,
		MASTER_PASSWORD_PASSED,
		SEARCH_MODE
	};

	struct SpecialPoints {
		MenuT::PointDescr<fd::FastDelegate0<>> formatFat;
	};

	struct FixedMenu {
		MenuT::PointDescr<CallbackT> settings;
		MenuT::PointDescr<CallbackT> exit;
	};

	static constexpr const char* KEEPASS_BASE_FILE = "db.kdb";
	static constexpr UsbKey TILDA_MODE_KEY = UsbKey::KEY_GRAVE_ACCENT_AND_TILDE;

	// Constructors
	TildaLogic(UsbDequeStandart* deque, const SpecialPoints& specialPoints);
	~TildaLogic();

	// Public methods
	void process(DataBufferConst inputData, size_t length);

private:
	SpecialPoints _specialMenuPoints;
	FixedMenu _fixedMenu;

	friend class Private::FixedMenuCallbacks;
	Private::FixedMenuCallbacks* _fixedMenuCbs;

	State _currentState;

	UsbKey _tildaKey;
	UsbSpecialKeySequence _tildaSeq;

	DataBufferConst _inputData;
	size_t _inputDataLength;
	UsbPackageConst* _inputPackagePtr;

	MenuTreeT _menuTree;
	MenuT _menu;

	PackageFactory _packageFactory;

	UsbPackage _lastPackage;
	KeyBuffer _keysBuffer;
	size_t _lastKeysBufferLen;
	KeyboardLikeInput<KeyBuffer> _keyboardInput;

	KeepAss::KeePassReader _keepassReader;
	DB::XmlTree _db;
	KeepAss::DecryptionResult _dbState;

	TildaLogic(const TildaLogic&);

	void _init();
	void _buildTree(uint32_t index);
	void _buildMenuTree();
	void _buildMenu();
	void _buildFixedMenu();
	template<typename T> void _addMenuPoint(T& point);
	void _copyDbContainer(uint32_t index);

	void _dbDecrypt(const char* passwd, size_t len);

	void _setState(State state) {
		_currentState = state;
	}

	void _logInCallback(MenuT::ContainerT& container);

	void _processMasterPassword();
	void _processMenuMode();
	const char* _getDbErrorType();

	void _checkMode();
	void _switchMenuMode();
	void _sendMenuGreeting();

	void _searchMode();

	void _redirectInput();
	void _sendMsg(const char* msg);
	void _clearMsg(const char* msg);
};

namespace Private {
	class FixedMenuCallbacks
	{
	public:
		FixedMenuCallbacks(TildaLogic* logic);

		void formatFat(TildaLogic::CallbackArg arg);
		void exit(TildaLogic::CallbackArg arg);

	private:
		TildaLogic* _logic;
	};
}

}  // namespace Logic

#endif /* TILDALOGIC_H_ */
