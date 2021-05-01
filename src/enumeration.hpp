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
#include "structs/dag/nodelist.hpp"
#include "structs/dag/node.hpp"
#include "factories/factories.hpp"
#include "memmanager.hpp"

namespace rematch {

class RegEx;

// Interface for enumerating the DAG-like structure obtained after
// the evaluation step (preprocessing the ourput). It uses the common
// methods next() and hasNext() for obtaining the outputs.
class Enumerator {

  friend class Evaluator;

  using SpanMap = std::map<std::string, std::pair<int64_t, int64_t>>;

  public:
    Enumerator(RegEx &rgx);

    // Get the next match according to the current state of the depth stack.
    Match_ptr next();

    // Returns if there is a next match according to the current state of
    // the depth stack.
    bool hasNext();

    // Adds a NodeList to the depth stack. So it's outputs get enumerated
    // in the future.
    void addNodeList(internal::NodeList &nL);

    // Resets the enumeration.
    void reset() {depth_stack_.clear();}

    // Returns the total number of mappings
    uint64_t numMappings() const {return nmappings_;}

 private:

  // Elements of the depth stack of the enumeration.
  struct EnumState {
    internal::Node* current_node;    // Current Node of the state of the enumeration.
    internal::Node* end_node;        // Needed to know when to stop while going down

    EnumState(internal::Node* c, internal::Node* e)
      : current_node(c), end_node(e) {}
  }; // end struct EnumeratorNode

  std::shared_ptr<VariableFactory> var_factory_;
  uint64_t nmappings_;

  std::vector<EnumState> depth_stack_;  // Stack for DFS in the mappingDAG

  // Store the current mapping because each next() call needs to
  // remember the capture positions already found.
  std::vector<int64_t> current_mapping_;
}; // end class Enumerator

} // end namespace rematch

#endif // ENUMERATION_H