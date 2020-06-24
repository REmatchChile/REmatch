#include <bitset>
#include <iostream>
#include <sstream>
#include <string>
#include "structures.hpp"


Node::Node()
  : S(0), i(-1), next(nullptr), start(nullptr), end(nullptr),
    refCount(0), nextFree(nullptr) {}


Node::Node(std::bitset<32> S, int i, Node* head, Node* tail)
  : S(S), i(i), next(nullptr), start(head), end(tail),
    refCount(0), nextFree(nullptr) {

    start->refCount++;
    end->refCount++;
  };

Node* Node::reset(std::bitset<32> S, int i, Node* head, Node* tail) {
  /* Same as constructor but inplace on an already constructed object */

   // Remove a refCount from head and tail of list

  std::cout << this->start<<'\n';
  this->start->refCount--;
  this->end->refCount--;

  if(next != nullptr) {
    next->refCount--;
  }

  this->S = S;
  this->i = i;
  this->start = head;
  this->end = tail;

  refCount = 0;
  // Don't reasign nextFree! memManager does that

  this->start->refCount++;
  this->end->refCount++;

  return this;
}

Node* Node::reset() {
   // Remove a refCount from head and tail of list
  this->start->refCount--;
  this->end->refCount--;

  if(next != nullptr) {
    next->refCount--;
  }

  this->S = 0;
  this->i = -1;
  this->start = nullptr;
  this->end = nullptr;

  refCount = 0;
  // Don't reasign nextFree! memManager does that

  return this;

}

void Node::getNodeContent(int content[2]) {
  // Get's the S and the i in the content array/
  content[0] = this->S.to_ulong();
  content[1] = this->i;
};

bool Node::isNodeEmpty() {
  return this->i == -1;
}

NodeList::NodeList()
  : head(nullptr), tail(nullptr), it(nullptr) {}

void NodeList :: start() {
  it = head;
}

Node* NodeList :: next() {
  it = it->next;
  return it;
}


void NodeList::setHead(Node* n) {
  this->head = n;
}

void NodeList::setTail(Node* n) {
  this->tail = n;
}

void NodeList::add(Node *node) {
  node->next = this->head;
  this->head = node;
  node->refCount++;
  if (this->tail == nullptr) {
    this->tail = node;
    node->refCount++;
  }
};

// NodeList* NodeList::lazycopy() {
//   NodeList* new_list = new NodeList();
//   new_list->head = this->head;
//   new_list->tail = this->tail;
//   return new_list;
// };

void NodeList::append(NodeList* list) {
  if (this->tail == nullptr) {
    this->head = list->head;
    this->tail = list->tail;

    list->head->refCount++;
    list->tail->refCount++;

  } else {
    this->tail->next = list->head;

    // Added reference from tail to list head
    list->head->refCount++;

    // Will remove reference from tail
    this->tail->refCount--;

    this->tail = list->tail;

    // Added reference to new tail
    list->tail->refCount++;
  }
};

void NodeList::resetRefs() {
  this->head->refCount--;
  this->tail->refCount--;
}

void NodeList::reset() {
  this->head = nullptr;
  this->tail = nullptr;
};

NodeList& NodeList :: operator=(const NodeList &nl) {
  head = nl.head;
  tail = nl.tail;

  return *this;
}


void NodeList :: resetAndAdd(Node* node) {
  node->next = nullptr;
  this->head = node;
  node->refCount += 2;
  this->tail = node;
}

bool NodeList :: empty() { return this->head == nullptr && this->tail == nullptr;}

std::string NodeList :: pprint(VariableFactory *vf) {
  std::stringstream ss;
  ss << "[";
  start();
  ss << "(" << vf->getVarUtil(it->S) << "," << it->i << ")" ;
  while(next() != nullptr) {
    ss << "; (" << vf->getVarUtil(it->S) << "," << it->i << ")" ;
  }
  ss << "]";
  return ss.str();
}