#ifndef STRUCTS_ECS_MINI_POOL_HPP
#define STRUCTS_ECS_MINI_POOL_HPP

#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include "output_enumeration/ecs_node.hpp"

namespace rematch {

class MiniPool {

private:
  size_t capacity_;
  std::vector<ECSNode> node_container;
  MiniPool *next_{nullptr};
  MiniPool *prev_{nullptr};

public:
  MiniPool(size_t cap);
  ~MiniPool();

  size_t capacity() const;
  size_t size() const;
  bool is_full() const;
  MiniPool *next() const;
  void set_next(MiniPool *mp);
  MiniPool *prev() const;
  void set_prev(MiniPool *mp);

  template <class... Args> ECSNode *alloc(Args... args) {
    node_container.emplace_back(std::forward<Args>(args)...);
    return &node_container.back();
  }


}; // end class MiniPool

} // end namespace rematch

#endif // STRUCTS_ECS_MINI_POOL_HPP
