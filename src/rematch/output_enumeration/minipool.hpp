#ifndef OUTPUT_ENUMERATION_MINI_POOL_HPP
#define OUTPUT_ENUMERATION_MINI_POOL_HPP

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include "output_enumeration/ecs_node.hpp"

namespace REmatch {

inline namespace output_enumeration {

/**
 * The NodeManager class stores its nodes through a linked list of MiniPools.
 */
class MiniPool {

 private:
  size_t capacity_;
  std::vector<ECSNode> node_container;
  MiniPool* next_{nullptr};
  MiniPool* prev_{nullptr};

 public:
  explicit MiniPool(size_t cap) : capacity_(cap) {
    node_container.reserve(capacity_);
  }

  inline size_t capacity() const { return node_container.capacity(); }
  inline size_t size() const { return node_container.size(); }
  inline bool is_full() const { return node_container.size() >= capacity_; }
  inline MiniPool* next() const { return next_; }
  inline void set_next(MiniPool* mp) { next_ = mp; }
  inline MiniPool* prev() const { return prev_; }
  inline void set_prev(MiniPool* mp) { prev_ = mp; }

  template <class... Args>
  inline ECSNode* alloc(Args... args) {
    node_container.emplace_back(std::forward<Args>(args)...);
    return &node_container.back();
  }
};
}  // namespace output_enumeration
}  // namespace REmatch

#endif
