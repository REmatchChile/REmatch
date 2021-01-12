#include "captures.hpp"

Capture::Capture(std::bitset<32> S, DetState* next): S(S), next(next) {}

Node* Capture::getNewNode(int i, NodeList* copiedList) {
  //NodeList* copiedList = this->list->lazycopy();
  return new Node(this->S, i, copiedList->head, copiedList->tail);
  //return new Node(this->S, i, this->list);
}
