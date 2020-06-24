#include "enumeration.hpp"

#include <iostream>
#include <sstream>
#include <bitset>

#include "match.hpp"
#include "structures.hpp"
#include "factories/factories.hpp"
#include "regex/regex.hpp"

namespace rematch {

Enumerator::Enumerator(RegEx &rgx, std::string &doc, MemManager &mem)
    : doc_(doc),
      mem_manager_(mem),
      rgx_(rgx),
      n_mappings_(0),
      data_(rgx_.varCount(), std::pair<size_t, size_t>(0, 0)) {}


void Enumerator::addNodeList(NodeList &startList) {
  if(!startList.empty()){
    std::map<std::string, std::pair<size_t,size_t>> ret;
    depth_stack_.emplace_back(startList.head, startList.tail);
    mem_manager_.addPossibleGarbage(startList.head);
  }
}

std::unique_ptr<Match> Enumerator :: next() {
  while(!depth_stack_.empty()) {
    auto current = depth_stack_.back();
    Node* node = current.current_node;

    depth_stack_.pop_back();

    if(node != current.end_node) {
      depth_stack_.emplace_back(node->next, current.end_node);
    }

    if(node->isNodeEmpty()) {
      n_mappings_++;
      return std::make_unique<Match>(&doc_, data_, rgx_.varScheme());
    }

    if(node->start != nullptr) {
      for(size_t j=0; j < data_.size(); j++) {
        if(node->S[2*j])
          data_[j].first = node->i;
        if(node->S[2*j+1])
          data_[j].second = node->i;
      }
      depth_stack_.emplace_back(node->start, node->end);
    }

  }

  throw exception();
}


bool Enumerator :: hasNext() {
  return !depth_stack_.empty();
}

} // end namespace rematch

