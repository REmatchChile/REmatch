#include "node_manager.hpp"

#include <cstdint>
#include <REmatch/exceptions.hpp>

namespace REmatch {
inline namespace output_enumeration {

NodeManager::NodeManager(size_t starting_size, uint_fast32_t max_mempool_duplications)
    : minipool_head_(new MiniPool(starting_size)),
      recyclable_node_head(nullptr),
      max_number_of_mempool_duplications(max_mempool_duplications) {}

NodeManager::NodeManager(uint_fast32_t max_mempool_duplications)
    : NodeManager(MEMORY_POOL_STARTING_SIZE, max_mempool_duplications) {}

NodeManager::~NodeManager() {
  for (MiniPool *mp = minipool_head_; mp != nullptr;) {
    MiniPool *next = mp->prev();
    delete mp;
    mp = next;
  }
}

void NodeManager::decrease_ref_count(ECSNode* node) {
  node->decrease_ref_count();
  try_to_mark_node_as_unused(node);
}

void NodeManager::increase_ref_count(ECSNode* node) {
  node->increase_ref_count();
}

void NodeManager::try_to_mark_node_as_unused(ECSNode* node) {
  if (node->ref_count == 0)
    add_to_list_of_free_memory(node);
}

void NodeManager::add_to_list_of_free_memory(ECSNode *node) {
    node->next_free_node = recyclable_node_head;
    recyclable_node_head = node;
}

size_t NodeManager::get_amount_of_nodes_reused() const {
  return amount_of_recycled_nodes;
}

size_t NodeManager::amount_of_nodes_allocated() const {
  size_t amount = 0;
  for(MiniPool *mpool = minipool_head_; mpool != nullptr; mpool = mpool->prev())
    amount += mpool->capacity();
  return amount;
}

ECSNode *NodeManager::
get_node_to_recycle_or_increase_mempool_size_if_necessary() {
  if(!minipool_head_->is_full())
    return nullptr;
  if (recyclable_node_head == nullptr) {
    increase_mempool_size();
    return nullptr;
  }
  return get_node_to_recycle();
}

void NodeManager::increase_mempool_size() {
  throw_exception_if_mempool_duplications_exceeded();
  MiniPool *new_minipool = new MiniPool(minipool_head_->size() * 2);
  minipool_head_->set_next(new_minipool);
  new_minipool->set_prev(minipool_head_);

  minipool_head_ = new_minipool;
}

void NodeManager::throw_exception_if_mempool_duplications_exceeded() {
  number_of_mempool_duplications++;
  if (number_of_mempool_duplications > max_number_of_mempool_duplications) {
    throw REmatch::MemoryLimitExceededException();
  }
}

ECSNode *NodeManager::get_node_to_recycle() {
    ECSNode *node_to_recycle = recyclable_node_head;
    ECSNode *children_of_the_recycled_node[] = {
      recyclable_node_head->right_node(),
      recyclable_node_head->left_node() };

    advance_recyclable_nodes_list_head();
    decrease_references_to_children(children_of_the_recycled_node);

    return node_to_recycle;
}

void NodeManager::decrease_references_to_children(
    ECSNode *children[2]) {
  for (int i=0; i<2; i++) {
    ECSNode *node = children[i];
    if (node != nullptr)
      decrease_ref_count(node);
  }
}

void NodeManager::advance_recyclable_nodes_list_head() {
    recyclable_node_head = recyclable_node_head->next_free_node;
    ++amount_of_recycled_nodes;
}

}
}
