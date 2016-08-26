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

#ifndef TREE_TREENODE_HPP_
#define TREE_TREENODE_HPP_

#include <string>
#include <functional>

#include <etl/vector.h>
#include <FastDelegate.h>

constexpr size_t MAX_CHILDREN_COUNT = 5;

namespace fd = fastdelegate;

template<typename T>
class TreeNode {
public:
	using TreeNodePtr = TreeNode*;
	using Container = T;

	using CallbackArg = Container&;
	using CallbackT = fd::FastDelegate1<CallbackArg>;

	static void EMPTY_CALLBACK(CallbackArg) {
		return;
	}

	TreeNode();
	TreeNode(std::string name, TreeNodePtr parent);
	~TreeNode();

	void setContainer(const Container& container) {
		_container = container;
	}

	void setCallback(const CallbackT& callback) {
		_callback = callback;
	}

	void call() {
		_callback(_container);
	}

	Container& getContainer() {
		return _container;
	}

	void setParent(TreeNodePtr parent) {
		_parent = parent;
	}

	TreeNodePtr getParent() {
		return _parent;
	}

	void addChild(TreeNodePtr child);
	TreeNodePtr getChildAt(const std::size_t i) const;

	std::size_t getChildrenCount() {
		return _childrenCount;
	}

	bool isEndNode() const {
		return (_children == nullptr);
	}

	void addLeftNeighbor(TreeNodePtr neighbor);
	void addRightNeighbor(TreeNodePtr neighbor);

	TreeNodePtr getLeftNeighbor() const {
		return _leftNeighbor;
	}

	TreeNodePtr getRightNeighbor() const {
		return _rightNeighbor;
	}

	friend void destroyTreeNode(TreeNode* node)
	{
		node->setParent(nullptr);
		node->_setRightNeighbor(nullptr);
		node->_setLeftNeighbor(nullptr);
		node->_children = nullptr;
		node->_childrenCount = 0;
	}

protected:
	Container _container;
    CallbackT _callback;

	TreeNodePtr _parent;
	std::size_t _childrenCount;
	TreeNodePtr _children;
	TreeNodePtr _leftNeighbor;
	TreeNodePtr _rightNeighbor;

	void _addFirstChild(TreeNodePtr child);
	void _setLeftNeighbor(TreeNodePtr neighbor);
	void _setRightNeighbor(TreeNodePtr neighbor);
};

template<typename T>
TreeNode<T>::TreeNode():
	_callback(EMPTY_CALLBACK),
	_parent(nullptr),
	_childrenCount(0),
	_children(nullptr),
	_leftNeighbor(nullptr),
	_rightNeighbor(nullptr)
{ }

template<typename T>
TreeNode<T>::TreeNode(std::string name, TreeNodePtr parent):
	_callback(EMPTY_CALLBACK),
	_parent(parent),
	_childrenCount(0),
	_children(nullptr),
	_leftNeighbor(nullptr),
	_rightNeighbor(nullptr)
{ }

template<typename T>
TreeNode<T>::~TreeNode()
{
	destroyTreeNode(this);
}

template<typename T>
inline void TreeNode<T>::_addFirstChild(TreeNodePtr child)
{
	_children = child;
}

template<typename T>
void TreeNode<T>::addChild(TreeNodePtr child)
{
	_childrenCount++;

	child->setParent(this);

	if (_childrenCount == 1) {
		_addFirstChild(child);
	}
	else {
		_children->addRightNeighbor(child);
	}
}

template<typename T>
void TreeNode<T>::addLeftNeighbor(TreeNodePtr neighbor)
{
	if (_leftNeighbor == nullptr) {
		_leftNeighbor = neighbor;
		neighbor->_setRightNeighbor(this);
	}
	else {
		_leftNeighbor->addLeftNeighbor(neighbor);
	}
}

template<typename T>
void TreeNode<T>::addRightNeighbor(TreeNodePtr neighbor)
{
	if (_rightNeighbor == nullptr) {
		_rightNeighbor = neighbor;
		neighbor->_setLeftNeighbor(this);
	}
	else {
		_rightNeighbor->addRightNeighbor(neighbor);
	}
}

template<typename T>
inline void TreeNode<T>::_setLeftNeighbor(TreeNodePtr neighbor)
{
	_leftNeighbor = neighbor;
}

template<typename T>
inline void TreeNode<T>::_setRightNeighbor(TreeNodePtr neighbor)
{
	_rightNeighbor = neighbor;
}

template<typename T>
typename TreeNode<T>::TreeNodePtr TreeNode<T>::getChildAt(const std::size_t childNum) const
{
	if (this->isEndNode()) {
		return nullptr;
	}

	std::size_t i = 0;
	TreeNodePtr child = _children;
	while (i < childNum) {
		child = child->getRightNeighbor();
		++i;
	}

	return child;
}

#endif /* TREE_TREENODE_HPP_ */
