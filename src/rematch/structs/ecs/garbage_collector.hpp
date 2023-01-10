#ifndef STRUCTS_ECS_GARBAGE_COLLECTOR_HPP
#define STRUCTS_ECS_GARBAGE_COLLECTOR_HPP

#include "minipool.hpp"
#include <sstream>
// Borré tot_arenas_ (no había funcionalidad en tot_arenas),
// id (se usa para hacer el print), i (sin funcionalidad),
// n_reused_nodes: no se actualizaba

namespace rematch {

const size_t MEMORY_POOL_STARTING_SIZE = 2048;

class GarbageCollector {

private:
  MiniPool *minipool_head_;
  size_t start_size_;

  size_t n_reused_nodes_{0};
  size_t n_nodes_{0};

  ECSNode *free_head_;


public:
  GarbageCollector(size_t starting_size = MEMORY_POOL_STARTING_SIZE)
      : minipool_head_(new MiniPool(starting_size)),
        start_size_(starting_size),
        free_head_(nullptr) {}

  template <class... Args> ECSNode *alloc(Args &&...args);

  void add_to_list_of_free_memory(ECSNode *node);

  std::string print_list_of_free_memory();

  size_t n_nodes() const { return n_nodes_; }

  size_t tot_size() const;

}; // end class GarbageCollector


} // end namespace rematch

#endif // STRUCTS_ECS_GARBAGE_COLLECTOR_HPP
