#include "node_manager.hpp"

namespace rematch {

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

std::string NodeManager::print_list_of_free_memory() {
    std::stringstream ss;
    auto head = recyclable_node_head;
    while(head != nullptr) {
      ss << "()" << ' ';
      head = head->next_free_node;
    }
    return ss.str();
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
    MiniPool *new_minipool = new MiniPool(minipool_head_->size() * 2);
    minipool_head_->set_next(new_minipool);
    new_minipool->set_prev(minipool_head_);

    minipool_head_ = new_minipool;
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

} // end namespace rematch
