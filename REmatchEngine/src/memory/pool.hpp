#ifndef MEMORY__POOL_HPP
#define MEMORY__POOL_HPP

// POOL
//
// Data structure that manages double-linked list of MiniPools while
// maintenaining a "free list" that points to a possible element stored in one
// of the MiniPools that can me overwritten by a new object.
//
// It's important to notice that the object of class T stored NEEDS to have a
// member variable of the type T* nextFree. So that the free list can work
// properly.
//
// TODO: Maybe we should implement a container template class for the stored
// objects so that the objects themselves don't need to have the "nextFree"
// member variable. My current problem with that aproach is that in the
// case of Node objects we use a reference counter that forms an union
// with the nextFree for memory efficiency.

#include <iostream>
#include <vector>
#include <queue>
#include <bitset>

#include "structures.hpp"
#include "memory/minipool.hpp"

// Starting capacity of the first MiniPool
size_t const STARTING_SIZE = 2048;

template <class T>
class Pool {
 public:
	Pool()
	  : current_(new MiniPool<T>(STARTING_SIZE)),
			free_head_(nullptr) {}

	// Allocate an object inside the Pool given it's constuction parameters.
	// Returns a pointer to the new object.
	template <typename ...Args>
	T* alloc(Args && ...args) {
		if(current_->is_full()) {
			if(free_head_) {
				update_free_list();
				T* old_head = free_head_;
				free_head_ = free_head_->nextFree;

				// Overwrites the old head and returns the new object
				return new (old_head) T(std::forward<Args>(args)...);
			} else {
				MiniPool<T> *new_minipool = new MiniPool<T>(current_->capacity() * 2);
				current_->set_next(new_minipool);
				new_minipool->set_prev(current_);
				current_ = new_minipool;
			}
		}

		return current_->alloc(std::forward<Args>(args)...);
	}

	void addFreeHead(T* elem) {
		elem->nextFree = free_head_;
		free_head_ = elem;
	}

	void addPossibleGarbage(T* node) {
		if(node->isNodeEmpty())
			std::cout << "Node empty...\n";
		if(node->refCount == 0 && !node->isNodeEmpty()) {
			addFreeHead(node);
		}
	}

	void reset() {
		MiniPool<T>* pool_ptr = current_, *aux;
		while(pool_ptr != nullptr) {
			aux = pool_ptr->getPrev();
			delete pool_ptr;
			pool_ptr = aux;
		}

		current_ = new MiniPool<T>(STARTING_SIZE),
		free_head_ = nullptr;
	}

 private:
	// Defaults to nothing. Use specialized templates functions if needed.
	void update_free_list() {}
	// Currently pointed minipool
	MiniPool<T> *current_;
	// Head of the free list
	T* free_head_;
};

#endif