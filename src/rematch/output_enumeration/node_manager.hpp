#pragma once

#include "output_enumeration/minipool.hpp"

namespace REmatch {
inline namespace output_enumeration {
using namespace REmatch;

const size_t MEMORY_POOL_STARTING_SIZE = 128;

/**
 * The Node Manager class stores the pointers to all allocated
 * ECSNode's. When an ECSNode is no longer used, i.e, when the amount
 * of references to it has become 0, that memory is available to be recycled.
 */
class NodeManager {

public:
  size_t amount_of_nodes_allocated() const;
  size_t amount_of_nodes_used{0};
  size_t amount_of_recycled_nodes{0};

private:
  MiniPool *minipool_head_;
  ECSNode *recyclable_node_head;
  size_t number_of_mempool_duplications = 0;
  size_t max_number_of_mempool_duplications = 0;

public:
  explicit NodeManager(size_t starting_size, uint_fast32_t max_mempool_duplications);
  explicit NodeManager(uint_fast32_t max_mempool_duplications);
  ~NodeManager();

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
  size_t get_amount_of_nodes_reused() const;

private:
  ECSNode *get_node_to_recycle_or_increase_mempool_size_if_necessary();
  void increase_mempool_size();
  void throw_exception_if_mempool_duplications_exceeded();
  ECSNode *get_node_to_recycle();
  void decrease_references_to_children(ECSNode *children[2]);
  void advance_recyclable_nodes_list_head();
  template <class... Args> ECSNode *allocate_a_new_node(Args &&...args) {
    ++amount_of_nodes_used;
    return minipool_head_->alloc(std::forward<Args>(args)...);
  }
  void try_to_mark_node_as_unused(ECSNode* node);

};
}
}
