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

#ifndef DATABASE_XMLTREE_XMLTREE_H_
#define DATABASE_XMLTREE_XMLTREE_H_

extern "C" {
#include <mxml.h>
}

#include <DbEntry.h>

namespace DB {

struct StringBool
{
	using BoolType = const StringFieldChar*;

	static constexpr BoolType TRUE = reinterpret_cast<BoolType>("True");
	static constexpr BoolType FALSE = reinterpret_cast<BoolType>("False");
};

class XmlTree {
public:
	using RawTree = mxml_node_t*;
	using RawNode = mxml_node_t*;

	struct NodeStruct {
		StringField name;
		StringField login;
		StringField password;
		StringField type;
		bool isExpanded;
	};

	XmlTree();
	XmlTree(RawTree tree);
	~XmlTree();

	void init(RawTree tree);

	const NodeStruct& getCurrentNodeInfo() {
		_updateNodeInfo();
		return _currentNodeStruct;
	}

	RawNode getCurrentNode() {
		return _currentNode;
	}

	void moveLeft();
	void moveRight();
	void moveInto();
	void moveOut();

	void moveInHead();
	void moveMostRight();
	void moveMostLeft();

	bool isMostLeft();
	bool isMostRight();
	bool isMostTop();
	bool isMostBottom();
	bool isEndNode();

private:
	struct TagStrings {
		using StringType = const StringFieldChar*;

		static constexpr StringType IS_EXPANDED =
				reinterpret_cast<StringType>("IsExpanded");

		static constexpr StringType NAME =
				reinterpret_cast<StringType>("Name");

		static constexpr StringType STRING =
				reinterpret_cast<StringType>("String");

		static constexpr StringType KEY =
				reinterpret_cast<StringType>("Key");

		static constexpr StringType VALUE =
				reinterpret_cast<StringType>("Value");

		static constexpr StringType ENTRY =
				reinterpret_cast<StringType>("Entry");

		static constexpr StringType GROUP =
				reinterpret_cast<StringType>("Group");
	};

	struct KeyStrings {
		using StringType = const StringFieldChar*;

		static constexpr StringType TITLE =
				reinterpret_cast<StringType>("Title");

		static constexpr StringType USER_NAME =
				reinterpret_cast<StringType>("UserName");

		static constexpr StringType PASSWORD =
				reinterpret_cast<StringType>("Password");

		static constexpr StringType TYPE =
				reinterpret_cast<StringType>("Type");
	};

	RawTree _rawTree;
	RawNode _currentNode;
	NodeStruct _currentNodeStruct;

	void _updateNodeInfo();

	RawNode _getNext();
	RawNode _getPrev();

	StringField _getKey(KeyStrings::StringType keyName);
	StringField _getTextByTag(TagStrings::StringType tagName);
	StringField _getText(RawNode node);

	RawNode _findElement(RawNode startNode, TagStrings::StringType elemName);
	RawNode _findElementInto(RawNode parentNode,
							 RawNode startNode,
							 TagStrings::StringType elemName);

	template<typename T>
	const char* _castInChar(T str) {
		return reinterpret_cast<const char*>(str);
	}
};

}

#endif /* DATABASE_XMLTREE_XMLTREE_H_ */
