#ifndef STRUCTS__ECS__MEMPOOL_HPP
#define STRUCTS__ECS__MEMPOOL_HPP

#include <cmath>
#include <vector>

namespace rematch {
namespace internal {

const size_t MEMORY_POOL_STARTING_SIZE = 2048;

template <class T>
class MiniPool {
 public:

  MiniPool(size_t cap): capacity_(cap), next_(nullptr), prev_(nullptr) {
    container_.reserve(capacity_);
  }

  bool is_full() const {return container_.size() >= capacity_;}

  size_t size() const {return container_.size();}
  size_t capacity() const {return container_.capacity();}

  MiniPool* next() const {return next_;}
  void set_next(MiniPool* mp) {next_ = mp;}
  MiniPool* prev() const {return prev_;}
  void set_prev(MiniPool* mp) {prev_ = mp;}

  template<class ...Args>
  T* alloc(Args... args) {
    container_.emplace_back(&args...);
    return &container_.back();
  }

 private:
  size_t capacity_;
  std::vector<T> container_;
  MiniPool* next_;
  MiniPool* prev_;

}; // end class MiniPool

template<class T>
class MemPool {
 public:
  MemPool(size_t starting_size = MEMORY_POOL_STARTING_SIZE)
      : minipool_head_(new MiniPool<T>(starting_size)),
        tot_arenas_(1),
        start_size_(starting_size),
        free_head_(nullptr) {}

  template<class ...Args>
  T* alloc(Args&& ...args) {
    if(minipool_head_->is_full()) {
      if(free_head_ != nullptr) {
        T* next_free = free_head_->next_free_;
        T* right_ptr = free_head_->right();
        T* left_ptr = free_head_->left();

        T* new_node = free_head_->reset(std::forward<Args>(args)...);

        if(left_ptr->ref_count_ == 0 && !left_ptr->is_empty()) {
          left_ptr->next_free_ = next_free;
          next_free = left_ptr;
        }
        if(!free_head_->is_output() && right_ptr->ref_count_ == 0 && !right_ptr->is_empty()) {
          right_ptr->next_free_ = next_free;
          next_free = right_ptr;
        }

        // Advance the freelist head
        free_head_ = next_free;

        return new_node;
      } else {
        MiniPool<T> *new_minipool = new MiniPool<T>(minipool_head_->size() * 2);
        minipool_head_->set_next(new_minipool);

        minipool_head_ = new_minipool;
        ++tot_arenas_;
      }
    }

    return minipool_head_->alloc(&args...);
  }

  size_t total_arenas() const { return tot_arenas_;}
  size_t total_size() const { return start_size_ * std::pow(2, tot_arenas_-1);}


 private:
  MiniPool<T> *minipool_head_;
  size_t tot_arenas_;
  size_t start_size_;
  T* free_head_;

}; // end class Pool

} // end namespace internal
} // end namespace rematch

#endif // STRUCTS__ECS__MEMPOOL_HPP