#ifndef STRUCTS_ECS_MINI_POOL_HPP
#define STRUCTS_ECS_MINI_POOL_HPP

#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include "ecs_node.hpp"

namespace rematch {

class MiniPool {
public:
  MiniPool(size_t cap) : capacity_(cap) {
    container_.reserve(capacity_);
  }

  bool is_full() const { return container_.size() >= capacity_; }

  size_t size() const { return container_.size(); }
  size_t capacity() const { return container_.capacity(); }

  MiniPool *next() const { return next_; }
  void set_next(MiniPool *mp) { next_ = mp; }
  MiniPool *prev() const { return prev_; }
  void set_prev(MiniPool *mp) { prev_ = mp; }

  template <class... Args> ECSNode *alloc(Args... args) {
    container_.emplace_back(std::forward<Args>(args)...);
    return &container_.back();
  }

private:
  size_t capacity_;
  std::vector<ECSNode> container_;
  MiniPool *next_{nullptr};
  MiniPool *prev_{nullptr};

}; // end class MiniPool

} // end namespace rematch

#endif // STRUCTS_ECS_MINI_POOL_HPP
