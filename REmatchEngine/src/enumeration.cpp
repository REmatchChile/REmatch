#include <iostream>
#include <sstream>
#include <bitset>

#include "match.hpp"
#include "enumeration.hpp"
#include "structures.hpp"
#include "factories/factories.hpp"



Enumerator :: Enumerator(std::string &doc, VariableFactory &vf, MemManager &mem):
    doc_(doc), vf_(vf), mem_manager_(mem), n_mappings_(0) {
      data_.resize(vf_.size());
      for(auto &elem: data_) {
        elem.fill(0);
      }
    }


void Enumerator :: addNodeList(NodeList &startList) {
  if(!startList.empty()){
    std::map<std::string, std::pair<size_t,size_t>> ret;
    depth_stack_.emplace_back(startList.head, startList.tail);
    mem_manager_.addPossibleGarbage(startList.head);
  }
}

rematch::Match Enumerator :: next() {
  while(!depth_stack_.empty()) {
    auto current = depth_stack_.back();
    Node* node = current.current_node;

    depth_stack_.pop_back();

    if(node != current.end_node) {
      depth_stack_.emplace_back(node->next, current.end_node);
    }

    if(node->isNodeEmpty()) {
      n_mappings_++;
      return rematch::Match(&doc_, data_, vf_.getOutputSchema());
    }

    if(node->start != nullptr) {
      // TODO: Implement FillPartialMapping inside enumeration
      vf_.fillPartialMapping(node->S, node->i, &data_);
      depth_stack_.emplace_back(node->start, node->end);
    }

  }

  throw exception();
}


bool Enumerator :: hasNext() {
  return !depth_stack_.empty();
}

