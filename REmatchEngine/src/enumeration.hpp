#ifndef ENUMERATION_H
#define ENUMERATION_H

#include <iostream>
#include <sstream>
#include <bitset>
#include <vector>
#include <string>
#include <stack>
#include <utility>
#include <memory>

#include "match.hpp"
#include "structures.hpp"
#include "factories/factories.hpp"
#include "memmanager.hpp"

namespace rematch {

class RegEx;

// Interface for enumerating the DAG-like structure obtained after
// the evaluation step (preprocessing the ourput). It uses the common
// methods next() and hasNext() for obtaining the outputs.
class Enumerator {

  friend class Evaluator;

  using SpanMap = std::map<std::string, std::pair<size_t, size_t>>;

  public:
    Enumerator(RegEx &rgx, std::string &doc);

    // Get the next match according to the current state of the depth stack.
    Match_ptr next();

    // Returns if there is a next match according to the current state of
    // the depth stack.
    bool hasNext();

    // Adds a NodeList to the depth stack. So it's outputs get enumerated
    // in the future.
    void addNodeList(NodeList &nL);

    // Resets the enumeration.
    void reset() {depth_stack_.clear();}

    // Returns the total number of mappings
    uint64_t numMappings() const {return n_mappings_;}

 private:

  // Elements of the depth stack of the enumeration.
  struct EnumState {
    Node* current_node;    // Current Node of the state of the enumeration.
    Node* end_node;        // Needed to know when to stop while going down

    EnumState(Node* c, Node* e)
      : current_node(c), end_node(e) {}

  }; // end struct EnumeratorNode


  std::string& doc_;                    // Sublaying document
  std::vector<EnumState> depth_stack_;  // Stack for DFS in the DAG-like struct


  RegEx& rgx_;

  uint64_t n_mappings_;                 // Total num of mappings

  std::vector<std::pair<size_t, size_t>> data_;

}; // end class Enumerator

} // end namespace rematch

#endif // ENUMERATION_H