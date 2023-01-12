#ifndef STRUCTS_ECS_GARBAGE_COLLECTOR_HPP
#define STRUCTS_ECS_GARBAGE_COLLECTOR_HPP

#include "minipool.hpp"
#include <sstream>
// Borré tot_arenas_ (no había funcionalidad en tot_arenas),
// id (se usa para hacer el print)
// n_reused_nodes: no se actualizaba

namespace rematch {

const size_t MEMORY_POOL_STARTING_SIZE = 2048;

class GarbageCollector {

public:
  size_t amount_of_nodes_allocated() const;
  size_t amount_of_nodes_used{0};
  size_t amount_of_recycled_nodes{0};

private:
  MiniPool *minipool_head_;
  ECSNode *recyclable_node_head;
  size_t initial_size;

public:
  GarbageCollector(size_t starting_size = MEMORY_POOL_STARTING_SIZE)
      : minipool_head_(new MiniPool(starting_size)),
        recyclable_node_head(nullptr),
        initial_size(starting_size) {}

  template <class... Args> ECSNode *alloc(Args &&...args) {
    ECSNode *recycled_node =
      get_node_to_recycle_or_increase_mempool_size_if_necessary();
    if (recycled_node != nullptr)
      return recycled_node->reset(std::forward<Args>(args)...);
    return allocate_a_new_node((args)...);
  }

  void decrease_ref_count(ECSNode* node);
  void increase_ref_count(ECSNode* node);

  void add_to_list_of_free_memory(ECSNode *node);
  size_t get_amount_of_nodes_used() const { return amount_of_nodes_used; }

  std::string print_list_of_free_memory();



private:
  ECSNode *get_node_to_recycle_or_increase_mempool_size_if_necessary();
  void increase_mempool_size();
  ECSNode *get_node_to_recycle();
  void decrease_references_to_children(ECSNode *children[2]);
  void advance_recyclable_nodes_list_head();
  template <class... Args> ECSNode *allocate_a_new_node(Args &&...args) {
    ++amount_of_nodes_used;
    return minipool_head_->alloc(std::forward<Args>(args)...);
  }
  void try_to_mark_node_as_unused(ECSNode* node);
}; // end class GarbageCollector


} // end namespace rematch

#endif // STRUCTS_ECS_GARBAGE_COLLECTOR_HPP
