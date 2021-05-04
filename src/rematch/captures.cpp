#include "captures.hpp"

namespace rematch {

Capture::Capture(std::bitset<32> S, DetState* next): S(S), next(next) {}

internal::Node* Capture::getNewNode(int i, internal::NodeList* copiedList) {
  //NodeList* copiedList = this->list->lazycopy();
  return new internal::Node(this->S, i, copiedList->head_, copiedList->tail_);
  //return new Node(this->S, i, this->list);
}

} // end namespace rematch
