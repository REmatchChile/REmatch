#include "enumeration.hpp"

#include <iostream>
#include <sstream>
#include <bitset>

#include "match.hpp"
#include "structs/dag/node.hpp"
#include "structs/dag/nodelist.hpp"
#include "structs/dag/fastnodelist.hpp"
#include "factories/factories.hpp"
#include "regex/regex.hpp"

namespace rematch {

Enumerator::Enumerator(RegEx &rgx)
    : var_factory_(rgx.detManager().varFactory()),
      nmappings_(0),
      current_mapping_(var_factory_->size() * 2, -1) {}


void Enumerator::addNodeList(internal::NodeList *startList) {
  if(!startList->empty()){
    std::map<std::string, std::pair<int64_t,int64_t>> ret;
    depth_stack_.emplace_back(startList->head_, startList->tail_);
  }
}

void Enumerator::addNodeList(internal::FastNodeList *startList) {
  if(!startList->empty()){
    std::map<std::string, std::pair<int64_t,int64_t>> ret;
    depth_stack_.emplace_back(startList->start(), startList->end());
  }
}

Match_ptr Enumerator::next() {
  while(!depth_stack_.empty()) {
    auto current = depth_stack_.back();
    internal::Node* node = current.current_node;

    depth_stack_.pop_back();

    if(node->isNodeEmpty()) {
      nmappings_++;
      std::unique_ptr<Match> ret(new Match(var_factory_, current_mapping_));
      return ret;
    }

    if(node != current.end_node) {
      depth_stack_.emplace_back(node->next, current.end_node);
    }

    if(node->start != nullptr) {
      for(size_t j=0; j < var_factory_->size() * 2; j++) {
        if(node->S[j])
          current_mapping_[j] =  node->i - var_factory_->get_offset(j);
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

