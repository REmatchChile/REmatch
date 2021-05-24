#include "nodelist.hpp"

namespace rematch {
namespace internal {

NodeList::NodeList()
  : head_(nullptr), tail_(nullptr) {}


void NodeList::set_head(Node* n) {
  this->head_ = n;
}

void NodeList::set_tail(Node* n) {
  this->tail_ = n;
}

void NodeList::add(Node *node) {
  node->next = this->head_;
  this->head_ = node;
  node->refCount++;
  if (this->tail_ == nullptr) {
    this->tail_ = node;
    node->refCount++;
  }
}

// NodeList* NodeList::lazycopy() {
//   NodeList* new_list = new NodeList();
//   new_list->head = this->head;
//   new_list->tail = this->tail;
//   return new_list;
// };

void NodeList::append(NodeList* list) {
  if(list->empty())
    return;
  if (this->tail_ == nullptr) {
    this->head_ = list->head_;
    this->tail_ = list->tail_;

    list->head_->refCount++;
    list->tail_->refCount++;

  } else {
    this->tail_->next = list->head_;

    // Added reference from tail to list head
    list->head_->refCount++;

    // Will remove reference from tail
    this->tail_->refCount--;

    this->tail_ = list->tail_;

    // Added reference to new tail
    list->tail_->refCount++;
  }
}

void NodeList::reset_refs() {
  if(!empty()) {
    this->head_->refCount--;
    this->tail_->refCount--;
  }
}

void NodeList::reset() {
  this->head_ = nullptr;
  this->tail_ = nullptr;
}

NodeList& NodeList :: operator=(const NodeList &nl) {
  head_ = nl.head_;
  tail_ = nl.tail_;

  return *this;
}


void NodeList :: reset_and_add(Node* node) {
  node->next = nullptr;
  this->head_ = node;
  node->refCount += 2;
  this->tail_ = node;
}

bool NodeList :: empty() { return this->head_ == nullptr && this->tail_ == nullptr;}

} // end namespace internal
} // end namespace rematch