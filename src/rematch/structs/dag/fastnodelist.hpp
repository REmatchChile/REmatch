#ifndef STRUCTS__DAG__FASTNODELIST_HPP
#define STRUCTS__DAG__FASTNODELIST_HPP

#include "structs/dag/node.hpp"

namespace rematch {
namespace internal {

class FastNodeList {
public:
  FastNodeList() : head_(new Node(Node::Type::kDummy)), tail_(head_) {}

  Node *start() { return head_->next; }

  Node *end() { return tail_; }

  bool empty() { return head_ == tail_; }

  void erase() {
    head_->next = nullptr;
    tail_ = head_;
  }

  void reset_refs() {
    // if(empty()) return;
    --head_->next->refCount;
    --tail_->refCount;
  }

  void add(Node *n) {
    tail_->next = n;
    --tail_->refCount;
    tail_ = n;
    n->refCount += 2;
  }
  void append(FastNodeList *l) {
    // if(l->empty()) return;
    tail_->next = l->head_->next;

    ++l->head_->next->refCount;
    --tail_->refCount;

    tail_ = l->tail_;
    ++l->tail_->refCount;
  }

private:
  Node *head_;
  Node *tail_;
};

} // end namespace internal
} // namespace rematch

#endif // STRUCTS__DAG__FASTNODELIST_HPP