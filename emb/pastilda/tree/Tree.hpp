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

#ifndef TREE_H_
#define TREE_H_

#include <string>
#include <functional>

#include <etl/pool.h>

#include <TreeNode.hpp>

template <std::size_t size, typename T>
class Tree {
public:
	static constexpr std::size_t NODES_COUNT = size;

	using TreeNodeT = TreeNode<T>;
	using TreeNodePtr = TreeNode<T>*;

	using ContainerT = typename TreeNodeT::Container;
	using CallbackT = typename TreeNodeT::CallbackT;
	using CallbackArg = typename TreeNodeT::CallbackArg;

	using PoolType = etl::pool<TreeNodeT, NODES_COUNT>;
	using PoolPtr = PoolType*;

	Tree();
	Tree(TreeNodePtr node, PoolPtr pool);
	virtual ~Tree();

	size_t getCurrentChildNum();

	void addChild(const CallbackT callback,
			      const CallbackArg container);
	void addChild(const CallbackT callback = &TreeNodeT::EMPTY_CALLBACK);
	void addNeighbor(const CallbackT callback,
		      	     const CallbackArg container);
	void addNeighbor(const CallbackT callback = &TreeNodeT::EMPTY_CALLBACK);

	void moveLeft();
	void moveRight();
	void moveInto();
	void moveOut();

	void moveInHead();
	void moveMostRight();
	void moveMostLeft();

	bool isMostLeft() {
		return (_currentNode->getLeftNeighbor() == nullptr);
	}

	bool isMostRight() {
		return (_currentNode->getRightNeighbor() == nullptr);
	}

	bool isMostTop() {
		return (_currentNode->getParent() == nullptr);
	}

	bool isMostBottom() {
		return (_currentNode->isEndNode());
	}

	TreeNodePtr getCurrentNode() {
		return _currentNode;
	}

	bool isFull() {
		return _pool->full();
	}

	void destroy();

private:
	PoolPtr _pool;
	TreeNodePtr _currentNode;

	void _destroyNode(TreeNodePtr node);
	void _destroyTree(TreeNodePtr tree);
};

template <std::size_t size, typename T>
Tree<size, T>::Tree() :
	_pool(new PoolType()),
	_currentNode(_pool->allocate())
{ }

template <std::size_t size, typename T>
Tree<size, T>::Tree(TreeNodePtr node, PoolPtr pool) :
	_pool(pool),
	_currentNode(node)
{ }

template <std::size_t size, typename T>
Tree<size, T>::~Tree()
{ }

template<std::size_t size, typename T>
size_t Tree<size, T>::getCurrentChildNum()
{
	TreeNodePtr node = _currentNode;
	size_t childNum = node->getParent()->getChildrenCount();

	while (node != nullptr) {
		childNum--;
		node = node->getLeftNeighbor();
	}

	childNum = _currentNode->getParent()->getChildrenCount() - childNum - 1;

	return childNum;
}

template<std::size_t size, typename T>
void Tree<size, T>::addChild(const CallbackT callback,
					  	  	 const CallbackArg container)
{
	if (this->isFull() == false) {
		TreeNodePtr child = _pool->allocate();
		child->setCallback(callback);
		child->setContainer(container);

		_currentNode->addChild(child);
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::addChild(const CallbackT callback)
{
	if (this->isFull() == false) {
		TreeNodePtr child = _pool->allocate();
		child->setCallback(callback);

		_currentNode->addChild(child);
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::addNeighbor(const CallbackT callback,
							    const CallbackArg container)
{
	if (this->isFull() == false) {
		TreeNodePtr neighbor = _pool->allocate();
		neighbor->setCallback(callback);
		neighbor->setContainer(container);

		_currentNode->addRightNeighbor(neighbor);
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::addNeighbor(const CallbackT callback)
{
	if (this->isFull() == false) {
		TreeNodePtr neighbor = _pool->allocate();
		neighbor->setCallback(callback);

		_currentNode->addRightNeighbor(neighbor);
	}
}


template<std::size_t size, typename T>
void Tree<size, T>::moveLeft()
{
	if (this->isMostLeft() == false) {
		_currentNode = _currentNode->getLeftNeighbor();
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::moveRight()
{
	if (this->isMostRight() == false) {
		_currentNode = _currentNode->getRightNeighbor();
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::moveInto()
{
	if (_currentNode->isEndNode() == false) {
		_currentNode = _currentNode->getChildAt(0);
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::moveOut()
{
	if (this->isMostTop() == false) {
		_currentNode = _currentNode->getParent();
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::destroy()
{
	_pool->release_all();
	_currentNode = _pool->allocate();
}

template<std::size_t size, typename T>
void Tree<size, T>::moveInHead()
{
	while (isMostTop() == false) {
		moveOut();
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::moveMostRight()
{
	while (isMostRight() == false) {
		moveRight();
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::moveMostLeft()
{
	while (isMostLeft() == false) {
		moveLeft();
	}
}

template<std::size_t size, typename T>
void Tree<size, T>::_destroyTree(TreeNodePtr tree)
{
	const std::size_t CHILDREN_COUNT = tree->getChildrenCount();
	for (std::size_t i = 0; i < CHILDREN_COUNT; ++i) {
		TreeNodePtr child = tree->getChildAt(CHILDREN_COUNT - i - 1);
		if (child->isEndNode()) {
			_destroyNode(child);
		}
		else {
			_destroyTree(child);
		}
	}
	_destroyNode(tree);
}

template<std::size_t size, typename T>
void Tree<size, T>::_destroyNode(TreeNodePtr node)
{
	destroyTreeNode(node);
}

#endif /* TREE_H_ */
