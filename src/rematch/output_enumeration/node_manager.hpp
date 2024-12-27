#pragma once

#include "output_enumeration/minipool.hpp"

#include <tracy/Tracy.hpp>

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
  MiniPool* minipool_head_;
  ECSNode* recyclable_node_head;
  size_t number_of_mempool_duplications = 0;
  size_t max_number_of_mempool_duplications = 0;

 public:
  explicit NodeManager(size_t starting_size,
                       uint_fast32_t max_mempool_duplications);
  explicit NodeManager(uint_fast32_t max_mempool_duplications);
  ~NodeManager();

  template <class... Args>
  inline ECSNode* alloc(Args&&... args) {
    ZoneScoped;
    ECSNode* recycled_node =
        get_node_to_recycle_or_increase_mempool_size_if_necessary();
    if (recycled_node != nullptr)
      return recycled_node->reset(std::forward<Args>(args)...);
    return allocate_a_new_node((args)...);
  }

  void decrease_ref_count(ECSNode* node);
  void increase_ref_count(ECSNode* node);

  size_t get_amount_of_nodes_used() const { return amount_of_nodes_used; }
  size_t get_amount_of_nodes_reused() const;

 private:
  ECSNode* get_node_to_recycle_or_increase_mempool_size_if_necessary();
  void increase_mempool_size();
  ECSNode* get_node_to_recycle();
  template <class... Args>
  inline ECSNode* allocate_a_new_node(Args&&... args) {
    ++amount_of_nodes_used;
    return minipool_head_->alloc(std::forward<Args>(args)...);
  }
};
}  // namespace output_enumeration
}  // namespace REmatch
