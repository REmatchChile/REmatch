#include <iostream>
#include <sstream>
#include <bitset>

#include "enumeration.hpp"
#include "structures.hpp"
#include "factories.hpp"



Enumerator :: Enumerator(std::ostream &os, VariableFactory* vf, MemManager* mem):
  startList(nullptr), vf(vf), memManager(mem), os(os), numOfMappings(0) {
    outputSchema = vf->getOutputSchema();
  }


void Enumerator :: addNodeList(NodeList &startList) {
  if(!startList.empty()){
    std::map<std::string, std::pair<size_t,size_t>> ret;
    depthStack.emplace(startList.head, ret, startList.tail);
    memManager->addPossibleGarbage(startList.head);
  }
}

std::map<std::string, std::pair<size_t,size_t>> 
Enumerator :: next() {
  while(!depthStack.empty()) {
    auto current = depthStack.top();
    Node* node = std::get<0>(current);
    std::string label = vf->getVarUtil(node->S);

    depthStack.pop();

    if(node != std::get<2>(current)) {
      // std::vector<pair_> mov(std::move(current.second));
      depthStack.push(std::make_tuple(node->next, std::get<1>(current), std::get<2>(current)));
    }

    if(node->isNodeEmpty()) {
      numOfMappings++;
      return std::get<1>(current);
    }

    if(node->start != nullptr) {
      auto ret(std::get<1>(current)); // Copy constructor
      vf->fillPartialMapping(ret, node->S, node->i);
      depthStack.emplace(node->start, ret, node->end);
    }

  }

  throw exception();
}


bool Enumerator :: hasNext() {
  return !depthStack.empty();
}

