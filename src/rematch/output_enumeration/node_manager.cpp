#include "node_manager.hpp"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

#include <REmatch/exceptions.hpp>
#include <cstdint>

namespace REmatch {
inline namespace output_enumeration {

NodeManager::NodeManager(size_t starting_size,
                         uint_fast32_t max_mempool_duplications)
    : minipool_head_(new MiniPool(starting_size)),
      recyclable_node_head(nullptr),
      max_number_of_mempool_duplications(max_mempool_duplications) {}

NodeManager::NodeManager(uint_fast32_t max_mempool_duplications)
    : NodeManager(MEMORY_POOL_STARTING_SIZE, max_mempool_duplications) {}

NodeManager::~NodeManager() {
  for (MiniPool* mp = minipool_head_; mp != nullptr;) {
    MiniPool* next = mp->prev();
    delete mp;
    mp = next;
  }
}

void NodeManager::decrease_ref_count(ECSNode* node) {
  node->decrease_ref_count();

  if (node->ref_count == 0) {
    node->next_free_node = recyclable_node_head;
    recyclable_node_head = node;
  }
}

void NodeManager::increase_ref_count(ECSNode* node) {
  node->increase_ref_count();
}

size_t NodeManager::get_amount_of_nodes_reused() const {
  return amount_of_recycled_nodes;
}

size_t NodeManager::amount_of_nodes_allocated() const {
  size_t amount = 0;
  for (MiniPool* mpool = minipool_head_; mpool != nullptr;
       mpool = mpool->prev())
    amount += mpool->capacity();
  return amount;
}

ECSNode*
NodeManager::get_node_to_recycle_or_increase_mempool_size_if_necessary() {
  if (!minipool_head_->is_full())
    return nullptr;
  if (recyclable_node_head == nullptr) {
    increase_mempool_size();
    return nullptr;
  }
  return get_node_to_recycle();
}
void NodeManager::increase_mempool_size() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif

  ++number_of_mempool_duplications;
  if (number_of_mempool_duplications > max_number_of_mempool_duplications) {
    throw REmatch::MemoryLimitExceededException();
  }

  MiniPool* new_minipool = new MiniPool(minipool_head_->size() * 2);
  minipool_head_->set_next(new_minipool);
  new_minipool->set_prev(minipool_head_);

  minipool_head_ = new_minipool;
}

ECSNode* NodeManager::get_node_to_recycle() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif

  ECSNode* node_to_recycle = recyclable_node_head;

  ECSNode* right_child = recyclable_node_head->right_node();
  ECSNode* left_child = recyclable_node_head->left_node();

  // Advance the recyclable node head
  recyclable_node_head = recyclable_node_head->next_free_node;
  ++amount_of_recycled_nodes;

  // Decrease the ref count of the children, starting with the right child
  if (right_child != nullptr) {
    decrease_ref_count(right_child);
  }
  if (left_child != nullptr) {
    decrease_ref_count(left_child);
  }

  return node_to_recycle;
}

}  // namespace output_enumeration
}  // namespace REmatch