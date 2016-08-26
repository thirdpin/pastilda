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

#ifndef MENU_H_
#define MENU_H_

#include <keys/Key.h>
#include <database/DbEntry.h>
#include <Tree.hpp>

template <std::size_t COUNT_OF_MENU_POINTS>
class Menu {
public:
	static constexpr std::size_t COUNT_OF_POINTS = COUNT_OF_MENU_POINTS;

	using TreeT = Tree<COUNT_OF_POINTS, DB::Entry>;
	using TreePtr = TreeT*;

	using MenuPoint = typename TreeT::TreeNodePtr;

	using ContainerT = typename TreeT::ContainerT;

	template<typename T>
	struct PointDescr {
		using CallbackT = T;

		const char* name;
		size_t nameLength;
		CallbackT callback;
	};

	void init(TreePtr tree) {
		_tree = tree;
	}
	void processKey(Keys::Key key);
	size_t search(DB::StringFieldConst& substring);

	void moveTop();

	ContainerT& getCurrentPointContainer();

	Menu();
	Menu(TreePtr tree);
	~Menu();

private:
	TreePtr _tree;

	void _processEnterKey();
};

template<std::size_t COUNT_OF_MENU_POINTS>
Menu<COUNT_OF_MENU_POINTS>::Menu():
	_tree(nullptr)
{ }

template<std::size_t COUNT_OF_MENU_POINTS>
Menu<COUNT_OF_MENU_POINTS>::Menu(TreePtr tree):
	_tree(tree)
{ }

template<std::size_t COUNT_OF_MENU_POINTS>
Menu<COUNT_OF_MENU_POINTS>::~Menu()
{ }

template<std::size_t COUNT_OF_MENU_POINTS>
inline void Menu<COUNT_OF_MENU_POINTS>::moveTop()
{
	_tree->moveInHead();  // maximum top
	_tree->moveMostLeft();  // maximum left
}

template<std::size_t COUNT_OF_MENU_POINTS>
void Menu<COUNT_OF_MENU_POINTS>::processKey(Keys::Key key)
{
	using ControlKey = Keys::UsbKey;

	if (key == ControlKey::KEY_UPARROW) {
		_tree->moveLeft();
	}
	else if (key == ControlKey::KEY_DOWNARROW) {
		_tree->moveRight();
	}
	else if (key == ControlKey::KEY_LEFTARROW) {
		_tree->moveOut();
	}
	else if (key == ControlKey::KEY_RIGHTARROW) {
		_tree->moveInto();
	}
	else if (key == ControlKey::KEY_ENTER) {
		_processEnterKey();
	}
}

template<std::size_t COUNT_OF_MENU_POINTS>
typename Menu<COUNT_OF_MENU_POINTS>::ContainerT&
Menu<COUNT_OF_MENU_POINTS>::getCurrentPointContainer()
{
	return _tree->getCurrentNode()->getContainer();
}

template<std::size_t COUNT_OF_MENU_POINTS>
void Menu<COUNT_OF_MENU_POINTS>::_processEnterKey()
{
	MenuPoint currentPoint = _tree->getCurrentNode();
	if (currentPoint->isEndNode()) {
		currentPoint->call();
	}
	else {
		_tree->moveInto();
	}
}

template<std::size_t COUNT_OF_MENU_POINTS>
size_t Menu<COUNT_OF_MENU_POINTS>::search(DB::StringFieldConst& substring)
{
	size_t pos = DB::StringField::npos;

	// Remember last node
	int32_t currentNodeNum = _tree->getCurrentChildNum();

	_tree->moveMostLeft();

	// Finding lambda
	auto findSubstring = [&]() -> size_t {
		DB::StringFieldConst& compareString =
				_tree->getCurrentNode()->getContainer().getName();
		pos = compareString.compare(0, substring.length(), substring);
	};

	// If found lambda
	auto substringWasFound = [&]() -> bool {
		return pos == 0;
	};

	// Trying to found something
	while (_tree->isMostRight() == false) {
		findSubstring();
		if (substringWasFound()) {
			break;
		}
		_tree->moveRight();
	}

	// Founding in last child
	if (substringWasFound() == false) {
		findSubstring();
	}

	// If found nothing, return to last node
	if (substringWasFound() == false) {
		_tree->moveMostLeft();
		while (currentNodeNum > 0) {
			currentNodeNum--;
			_tree->moveRight();
		}
	}

	return pos;
}

#endif /* MENU_H_ */
