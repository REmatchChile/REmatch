#ifndef STRUCTS_ECS_MEMPOOL_HPP
#define STRUCTS_ECS_MEMPOOL_HPP

#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

namespace rematch {

const size_t MEMORY_POOL_STARTING_SIZE = 2048;

template <class T> class MiniPool {
public:
  MiniPool(size_t cap) : capacity_(cap) {
    container_.reserve(capacity_);
  }

  bool is_full() const { return container_.size() >= capacity_; }

  size_t size() const { return container_.size(); }
  size_t capacity() const { return container_.capacity(); }

  MiniPool *next() const { return next_; }
  void set_next(MiniPool *mp) { next_ = mp; }
  MiniPool *prev() const { return prev_; }
  void set_prev(MiniPool *mp) { prev_ = mp; }

  template <class... Args> T *alloc(Args... args) {
    container_.emplace_back(std::forward<Args>(args)...);
    return &container_.back();
  }

private:
  size_t capacity_;
  std::vector<T> container_;
  MiniPool *next_{nullptr};
  MiniPool *prev_{nullptr};

}; // end class MiniPool

template <class T> class MemPool {
public:
  MemPool(size_t starting_size = MEMORY_POOL_STARTING_SIZE)
      : minipool_head_(new MiniPool<T>(starting_size)),
        tot_arenas_(1),
        start_size_(starting_size),
        free_head_(nullptr) {}

  template <class... Args> T *alloc(Args &&...args) {
    #ifdef NOPT_MEMORY
    return new T(std::forward<Args>(args)...);
    #else
    if (minipool_head_->is_full()) {
      if (free_head_ != nullptr) {
        T *old_free_head = free_head_;
        T *next_free = free_head_->next_free_;
        T *right_ptr = free_head_->right();
        T *left_ptr = free_head_->left();

        // Need to decrease the ref_count_ of left and right.
        --left_ptr->ref_count_;
        if (right_ptr != nullptr)
          --right_ptr->ref_count_;

        if (left_ptr->ref_count_ == 0 && !left_ptr->is_bottom()) {
          left_ptr->next_free_ = next_free;
          next_free = left_ptr;
        }
        if (!free_head_->is_output() && right_ptr->ref_count_ == 0 &&
            !right_ptr->is_bottom()) {
          right_ptr->next_free_ = next_free;
          next_free = right_ptr;
        }

        // Advance the freelist head
        free_head_ = next_free;
        ++n_reused_nodes_;

        return old_free_head->reset(std::forward<Args>(args)...);
      } else {
        MiniPool<T> *new_minipool = new MiniPool<T>(minipool_head_->size() * 2);
        minipool_head_->set_next(new_minipool);
        new_minipool->set_prev(minipool_head_);

        minipool_head_ = new_minipool;
      }
    }

    ++n_nodes_;

    return minipool_head_->alloc(std::forward<Args>(args)...);
    #endif // NOPT_MEMORY
  }

  // Adds a node to the free list. It's assumed that the node has ref_count_ ==
  // 0.
  void add_to_free_list(T *node) {
    node->next_free_ = free_head_;
    free_head_ = node;
  }

  std::string print_free_list() {
    std::stringstream ss;
    auto head = free_head_;
    while(head != nullptr) {
      ss << head->id_ << ' ';
      head = head->next_free_;
    }
    return ss.str();
  }

  size_t total_arenas() const { return tot_arenas_; }

  size_t n_nodes() const { return n_nodes_; }
  size_t n_reused_nodes() const { return n_reused_nodes_; }

  size_t tot_size() const {
    size_t res = 0;
    for(MiniPool<T> *mpool = minipool_head_; mpool != nullptr; mpool = mpool->prev()) {
      res += sizeof(*mpool) + mpool->size() * sizeof(T);
    }

    return sizeof(*this) + res;
  }


private:
  MiniPool<T> *minipool_head_;
  size_t tot_arenas_;
  size_t start_size_;

  size_t n_reused_nodes_{0};
  size_t n_nodes_{0};

  T *free_head_;

}; // end class Pool


template<typename T>
class FakeMemPool {
 public:
  template <class... Args> 
  T *alloc(Args &&...args) {
    nnodes_++;
    return new T(std::forward<Args>(args)...);
  }

  void add_to_free_list(T*) {}

  size_t n_nodes() const { return nnodes_; }
  size_t n_reused_nodes() const { return 0; }
  size_t tot_size() const { return 0;}

 private:
  size_t nnodes_{0};


}; // end class FakeMemPool

} // end namespace rematch

#endif // STRUCTS_ECS_MEMPOOL_HPP
