#include "enumeration.hpp"

#include <iostream>
#include <sstream>
#include <bitset>

#include "match.hpp"
#include "structs/dag/node.hpp"
#include "structs/dag/nodelist.hpp"
#include "factories/factories.hpp"
#include "regex/regex.hpp"

namespace rematch {

Enumerator::Enumerator(RegEx &rgx, std::string &doc)
    : doc_(doc),
      rgx_(rgx),
      n_mappings_(0),
      data_(rgx_.varCount(), std::pair<int64_t, int64_t>(0, 0)) {}


void Enumerator::addNodeList(internal::NodeList &startList) {
  if(!startList.empty()){
    std::map<std::string, std::pair<int64_t,int64_t>> ret;
    depth_stack_.emplace_back(startList.head_, startList.tail_);
  }
}

Match_ptr Enumerator :: next() {
  while(!depth_stack_.empty()) {
    auto current = depth_stack_.back();
    internal::Node* node = current.current_node;

    depth_stack_.pop_back();

    if(node->isNodeEmpty()) {
      n_mappings_++;
      return std::make_unique<Match>(doc_, data_, rgx_.varScheme());
    }

    if(node != current.end_node) {
      depth_stack_.emplace_back(node->next, current.end_node);
    }

    if(node->start != nullptr) {
      for(size_t j=0; j < data_.size(); j++) {
        if(node->S[2*j])
          data_[j].first = node->i - rgx_.detManager().varFactory()->getOffset(2*j);
        if(node->S[2*j+1])
          data_[j].second = node->i - rgx_.detManager().varFactory()->getOffset(2*j+1);
      }
      depth_stack_.emplace_back(node->start, node->end);
    }

  }

  throw std::exception();
}


bool Enumerator :: hasNext() {
  return !depth_stack_.empty();
}

} // end namespace rematch

