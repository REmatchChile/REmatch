#include "output_enumeration/minipool.hpp"

namespace REmatch {
inline namespace output_enumeration {

MiniPool::MiniPool(size_t cap) : capacity_(cap) {
  node_container.reserve(capacity_);
}

size_t MiniPool::capacity() const { return node_container.capacity(); }
size_t MiniPool::size() const { return node_container.size(); }

bool MiniPool::is_full() const { return node_container.size() >= capacity_; }
MiniPool *MiniPool::next() const { return next_; }
void MiniPool::set_next(MiniPool *mp) { next_ = mp; }
MiniPool *MiniPool::prev() const { return prev_; }
void MiniPool::set_prev(MiniPool *mp) { prev_ = mp; }

}
}
