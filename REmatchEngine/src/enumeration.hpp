#ifndef ENUMERATION_HPP
#define ENUMERATION_HPP

#include <iostream>
#include <sstream>
#include <bitset>
#include <vector>
#include <string>
#include <stack>
#include <utility>

#include "structures.hpp"
#include "factories.hpp"
#include "memmanager.hpp"



class Enumerator {
  public:
    NodeList* startList;
    VariableFactory* vf;
    MemManager* memManager;

    std::ostream &os;

    size_t numOfMappings;

    std::vector<std::string> outputSchema;

    std::stack<
      std::tuple<
        Node*,
        std::map<std::string, std::pair<size_t,size_t>>,
        Node* >> depthStack;

    Enumerator(std::ostream &os, VariableFactory* vf, MemManager* mem);

    std::map<std::string, std::pair<size_t,size_t>> next();
    bool hasNext();
    void addNodeList(NodeList &nL);
};

#endif