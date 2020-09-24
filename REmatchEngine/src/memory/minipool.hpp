#ifndef MEMORY__MINIPOOL_HPP
#define MEMORY__MINIPOOL_HPP

// Minipool.h
// Notice: This class is header-only beacause of the usage of templates.

#include <vector>

template <class T>
class MiniPool {

 public:
  // Construct only with the capacity reserve.
	MiniPool(size_t size)
    : capacity_(size), next_(nullptr), prev_(nullptr) {
    data_.reserve(capacity_);
  }

	~MiniPool() = default;

  // Check if current container is at max capacity
	bool is_full() const {return data_.size() >= capacity_;}

  // Allocate an object inside the container and return its pointer.
  template <typename ...Args>
	T* alloc(Args && ...args) {
		data_.emplace_back(std::forward<Args>(args)...);
		return &data_.back();
	}

  // Setters

	void set_next(MiniPool *m) {next_ = m;}
	void set_prev(MiniPool *m) {prev_ = m;}

  // Getters

	MiniPool* prev() const {return prev_;}
  MiniPool* next() const {return next_;}
	size_t capacity() const {return capacity_;}

 private:
  // Total capacity of the minipool
	size_t capacity_;

  // Vector of stored data
	std::vector<T> data_;

  // Next minipool
	MiniPool *next_;

  // Previous minipool
	MiniPool *prev_;

};

#endif