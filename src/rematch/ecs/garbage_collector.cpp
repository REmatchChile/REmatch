#include "garbage_collector.hpp"

namespace rematch {

template <class... Args> ECSNode *GarbageCollector::alloc(Args &&...args) {
  if (minipool_head_->is_full()) {
    if (free_head_ != nullptr) {
      ECSNode *old_free_head = free_head_;
      ECSNode *next_free = free_head_->next_free_node;
      ECSNode *right_ptr = free_head_->right_node();
      ECSNode *left_ptr = free_head_->left_node();

      // Need to decrease the ref_count of left and right.
      --left_ptr->ref_count;
      if (right_ptr != nullptr)
        --right_ptr->ref_count;

      if (left_ptr->ref_count == 0 && !left_ptr->is_bottom()) {
        left_ptr->next_free_node = next_free;
        next_free = left_ptr;
      }
      if (!free_head_->is_output() && right_ptr->ref_count == 0 &&
          !right_ptr->is_bottom()) {
        right_ptr->next_free_node = next_free;
        next_free = right_ptr;
      }

      // Advance the freelist head
      free_head_ = next_free;
      ++n_reused_nodes_;

      return old_free_head->reset(std::forward<Args>(args)...);
    } else {
      MiniPool *new_minipool = new MiniPool(minipool_head_->size() * 2);
      minipool_head_->set_next(new_minipool);
      new_minipool->set_prev(minipool_head_);

      minipool_head_ = new_minipool;
    }
  }

  ++n_nodes_;

  return minipool_head_->alloc(std::forward<Args>(args)...);
}

void GarbageCollector::add_to_list_of_free_memory(ECSNode *node) {
    node->next_free_node = free_head_;
    free_head_ = node;
}

std::string GarbageCollector::print_list_of_free_memory() {
    std::stringstream ss;
    auto head = free_head_;
    while(head != nullptr) {
      ss << "()" << ' ';
      head = head->next_free_node;
    }
    return ss.str();
}

size_t GarbageCollector::tot_size() const {
  size_t res = 0;
  for(MiniPool *mpool = minipool_head_; mpool != nullptr; mpool = mpool->prev())
    res += sizeof(*mpool) + mpool->size() * sizeof(ECSNode);

  return sizeof(*this) + res;
}

}
