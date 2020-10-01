#ifndef MEMMANAGER_HPP
#define MEMMANAGER_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <bitset>
#include "structures.hpp"

size_t const STARTING_SIZE = 2048;

class MiniPool {

 private:
	size_t m_capacity;
	std::vector<Node> minipool;
	MiniPool *next;
	MiniPool *prev;

 public:

	MiniPool(size_t size): m_capacity(size), next(nullptr), prev(nullptr) {
		minipool.reserve(m_capacity);
	}

	~MiniPool() = default;

	bool isFull() const {return minipool.size() >= m_capacity;}

	Node* alloc() {

		minipool.emplace_back();

		return &minipool.back();
	}

	Node* alloc(std::bitset<32> S, int i, Node *head, Node *tail) {

		minipool.emplace_back(S, i, head, tail);

		return &minipool.back();
	}

	void setNext(MiniPool *m) {next = m;}

	void setPrev(MiniPool *m) {prev = m;}

	MiniPool* getPrev() {return prev;}

	size_t size() const {return m_capacity;}


};


class MemManager {
private:

	MiniPool *current;
	Node* freeHead;

	size_t totElements;
	size_t totArenas;
	size_t totElementsReused;

public:

	MemManager()
	  : current(new MiniPool(STARTING_SIZE)), freeHead(nullptr), totElements(0),
		totArenas(1), totElementsReused(0) {}

	Node* alloc() {

	if(current->isFull()) {
		// GARBAGE COLLECTION
		if(freeHead != nullptr) {
			Node *listHead, *adyacentNext, *newNode;

			// Pointer labeling
			listHead = freeHead->start;
			adyacentNext = freeHead->next;

			// Overwrite Node pointed by freeHead
			newNode = freeHead->reset();

			// Append to freelist new garbage
			if(listHead != nullptr && listHead->refCount == 0) {
				listHead->nextFree = freeHead->nextFree;
				freeHead->nextFree = listHead;
			}
			if(adyacentNext != nullptr && adyacentNext->refCount == 0) {
				adyacentNext->nextFree = freeHead->nextFree;
				freeHead->nextFree = adyacentNext;
			}

			// Reassign freeHead
			freeHead = freeHead->nextFree;

			// Reset nextFree in overwritten Node
			newNode->nextFree = nullptr;
			totElementsReused++;

			return newNode;
		}
		else {
			MiniPool *new_minipool = new MiniPool(current->size() * 2);
			current->setNext(new_minipool);
			new_minipool->setPrev(current);

			current = new_minipool;
			totArenas++;

		}

	}

	totElements++;

	return current->alloc();

	}

	Node* alloc(std::bitset<32> S, int i, Node *head, Node *tail) {

	if(current->isFull()) {
		// GARBAGE COLLECTION
		if(freeHead != nullptr) {
			Node *listHead, *adyacentNext, *newNode;

			// Pointer labeling
			listHead = freeHead->start;
			adyacentNext = freeHead->next;

			// Overwrite Node pointed by freeHead
			newNode = freeHead->reset(S, i, head, tail);

			// Append to freelist new garbage
			if(listHead != nullptr && listHead->refCount == 0 && !listHead->isNodeEmpty()) {
				listHead->nextFree = freeHead->nextFree;
				freeHead->nextFree = listHead;
			}
			if(adyacentNext != nullptr && adyacentNext->refCount == 0 && !listHead->isNodeEmpty()) {
				adyacentNext->nextFree = freeHead->nextFree;
				freeHead->nextFree = adyacentNext;
			}

			// Reassign freeHead
			freeHead = freeHead->nextFree;

			// Reset nextFree in overwritten Node
			newNode->nextFree = nullptr;
			totElementsReused++;

			return newNode;
		}
		else {
			MiniPool *new_minipool = new MiniPool(current->size() * 2);
			current->setNext(new_minipool);

			current = new_minipool;
			totArenas++;

		}

	}

	totElements++;

	return current->alloc(S, i, head, tail);

	}

	void addFreeHead(Node* n) {
		n->nextFree = freeHead;
		freeHead = n;
	}

	void addPossibleGarbage(Node* node) {
		if(node->isNodeEmpty())
			// std::cout << "Node empty...\n";
		if(node->refCount == 0 && !node->isNodeEmpty()) {
			addFreeHead(node);
		}
	}

	size_t totNodes() { return totElements;}

	size_t totNodeArenas() {return totArenas;}

	size_t totNodesReused() { return totElementsReused;}

	void reset() {
		MiniPool* pool_ptr = current, *aux;
		while(pool_ptr != nullptr) {
			aux = pool_ptr->getPrev();
			delete pool_ptr;
			pool_ptr = aux;
		}

		current = new MiniPool(STARTING_SIZE),
		freeHead = nullptr;
		totElements = 0;
		totArenas = 1;
		totElementsReused = 0;
	}

};

#endif