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
#include <cstdint>
#include <cstring>

#include <etl/string.h>

#include "XmlTree.h"

using std::size_t;

namespace DB {

XmlTree::XmlTree(RawTree tree)
{
	init(tree);
}

XmlTree::XmlTree():
	_rawTree(nullptr),
	_currentNode(nullptr)
{ }

void XmlTree::init(RawTree tree)
{
	// Finding the top Group tag (it should be in file)
	RawNode topNode =
			mxmlFindElement(tree, tree,
						    _castInChar(TagStrings::GROUP), NULL, NULL,
							MXML_DESCEND);

	_currentNode = topNode;
	_rawTree = topNode;

	moveInto();  // avoid top group
	_updateNodeInfo();
}

XmlTree::~XmlTree() { }

void XmlTree::moveInto()
{
	if (isMostBottom() == false) {
		RawNode entryNode = _findElement(_currentNode, TagStrings::ENTRY);
		RawNode groupNode = _findElement(_currentNode, TagStrings::GROUP);

		// Entry nodes are always first in keepass xml file
		if (entryNode != NULL) {
			_currentNode = entryNode;
		}
		else if (groupNode != NULL) {
			_currentNode = groupNode;
		}
	}
}

void XmlTree::moveOut()
{
	if (isMostTop() == false) {
		_currentNode = mxmlGetParent(_currentNode);
	}
}

void XmlTree::moveLeft()
{
	if (isMostLeft() == false) {
		_currentNode = _getPrev();
	}
}

void XmlTree::moveRight()
{
	if (isMostRight() == false) {
		_currentNode = _getNext();
	}
}

void XmlTree::moveInHead()
{
	_currentNode = _rawTree;
	moveInto();  // avoid top group
}

void XmlTree::moveMostLeft()
{
	while (isMostLeft() == false) {
		moveLeft();
	}
}

void XmlTree::moveMostRight()
{
	while (isMostRight() == false) {
		moveRight();
	}
}

bool XmlTree::isEndNode()
{
	_updateNodeInfo();
	return !_currentNodeStruct.isExpanded;
}

bool XmlTree::isMostTop()
{
	return (mxmlGetParent(_currentNode) == _rawTree);
}

bool XmlTree::isMostBottom()
{
	if (isEndNode() == false) {
		RawNode entryNode = _findElement(_currentNode, TagStrings::ENTRY);
		RawNode groupNode = _findElement(_currentNode, TagStrings::GROUP);
		return (entryNode == NULL && groupNode == NULL);
	}

	return true;
}

bool XmlTree::isMostLeft()
{
	return (_getPrev() == NULL);
}

bool XmlTree::isMostRight()
{
	return (_getNext() == NULL);
}

auto XmlTree::_getNext() -> RawNode
{
	// We should to mxmlWalkNext() twice,
	// because min-xml lib working so.
	RawNode node =
		mxmlWalkNext(_currentNode, mxmlGetParent(_currentNode), MXML_NO_DESCEND);
	node =
		mxmlWalkNext(node, mxmlGetParent(_currentNode), MXML_NO_DESCEND);

	return node;
}

auto XmlTree::_getPrev() -> RawNode
{
	// We should to mxmlWalkPrev() twice,
	// because min-xml lib working so.
	RawNode node =
		mxmlWalkPrev(_currentNode, mxmlGetParent(_currentNode), MXML_NO_DESCEND);
	node =
		mxmlWalkPrev(node, mxmlGetParent(_currentNode), MXML_NO_DESCEND);

	// Check if it's Entry or Group
	const char* tagName = node->value.element.name;
	if (std::strcmp(tagName, _castInChar(TagStrings::ENTRY)) == 0 ||
		std::strcmp(tagName, _castInChar(TagStrings::GROUP)) == 0)
	{
		return node;
	}

	return NULL;
}

void XmlTree::_updateNodeInfo()
{
	StringField fieldIsExpanded = _getTextByTag(TagStrings::IS_EXPANDED);

	bool isExpanded = (fieldIsExpanded == StringBool::TRUE);
	_currentNodeStruct.isExpanded = isExpanded;

	if (isExpanded) {  // if <Group>
		_currentNodeStruct.name = _getTextByTag(TagStrings::NAME);
		_currentNodeStruct.login =
		_currentNodeStruct.password =
		_currentNodeStruct.type = DB::EMPTY_FIELD;
	}
	else {  // if <Entry>
		_currentNodeStruct.name = _getKey(KeyStrings::TITLE);
		_currentNodeStruct.login = _getKey(KeyStrings::USER_NAME);
		_currentNodeStruct.password = _getKey(KeyStrings::PASSWORD);
		_currentNodeStruct.type = _getKey(KeyStrings::TYPE);
	}
}

StringField XmlTree::_getTextByTag(TagStrings::StringType tagName)
{
	RawNode nameNode = _findElement(_currentNode, tagName);
	if (nameNode == nullptr) {
		return DB::EMPTY_FIELD;
	}

	return _getText(nameNode);
}

auto XmlTree::_findElement(RawNode startNode,
						   TagStrings::StringType elemName) -> RawNode
{
	if (startNode == _currentNode) {
		startNode = _currentNode->child;
	}
	return mxmlFindElement(startNode, _currentNode,
						   _castInChar(elemName), NULL, NULL,
						   MXML_NO_DESCEND);
}

auto XmlTree::_findElementInto(RawNode parentNode,
							   RawNode startNode,
							   TagStrings::StringType elemName) -> RawNode
{
	if (startNode == NULL) {
		startNode = parentNode->child;
	}
	return mxmlFindElement(startNode, parentNode,
						   _castInChar(elemName), NULL, NULL,
						   MXML_NO_DESCEND);
}

StringField XmlTree::_getText(RawNode node)
{
	const char* text = mxmlGetOpaque(node);
	size_t length = etl::strlen(text);

	return StringField(
			reinterpret_cast<const StringFieldChar*>(text),
			length
		);
}

StringField XmlTree::_getKey(KeyStrings::StringType keyName)
{
    for (RawNode stringNode = _findElement(_currentNode, TagStrings::STRING);
         stringNode != NULL;
         stringNode = _findElement(stringNode, TagStrings::STRING))
    {
    	RawNode keyNode = _findElementInto(stringNode, NULL, TagStrings::KEY);
    	StringField keyText = _getText(keyNode);
    	if (keyText == keyName) {
    		RawNode valueNode =
    				_findElementInto(stringNode, keyNode, TagStrings::VALUE);
    		return _getText(valueNode);
    	}
    }

    return StringField();
}

}
