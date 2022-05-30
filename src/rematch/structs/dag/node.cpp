#include "node.hpp"

#include <queue>

namespace rematch {
namespace internal {

size_t Node::ID = 2;

Node::Node(Type t)
    : S(0), i(-1), next(nullptr), start(nullptr), end(nullptr), refCount(0) {
  if (t == Type::kBottom) {
    id_ = 1;
    i = -1;
  } else if (t == Type::kDummy) {
    id_ = 0;
    i = -2;
  }
}

Node::Node(std::bitset<32> S, int64_t i, Node *head, Node *tail)
    : id_(Node::ID++), S(S), i(i), next(nullptr), start(head), end(tail),
      refCount(0) {

  start->refCount++;
  end->refCount++;
}

Node *Node::reset(std::bitset<32> S, int64_t i, Node *head, Node *tail) {
  /* Same as constructor but inplace on an already constructed object */

  // Remove a refCount from head and tail of list

  // std::cout << this->start<<'\n';
  this->start->refCount--;
  this->end->refCount--;

  if (next != nullptr) {
    next->refCount--;
  }

  this->S = S;
  this->i = i;
  this->start = head;
  this->end = tail;
  this->next = nullptr;

  id_ = Node::ID++;

  // Don't reasign nextFree! memManager does that

  this->start->refCount++;
  this->end->refCount++;

  return this;
}

Node *Node::reset() {
  // Remove a refCount from head and tail of list
  this->start->refCount--;
  this->end->refCount--;

  if (next != nullptr) {
    next->refCount--;
  }

  this->S = 0;
  this->i = -1;
  this->start = nullptr;
  this->end = nullptr;

  id_ = Node::ID++;

  refCount = 0;
  // Don't reasign nextFree! memManager does that

  return this;
}

void Node::getNodeContent(int content[2]) {
  // Get's the S and the i in the content array/
  content[0] = this->S.to_ulong();
  content[1] = this->i;
}

bool Node::isNodeEmpty() { return this->i == -1; }

} // end namespace internal
} // end namespace rematch