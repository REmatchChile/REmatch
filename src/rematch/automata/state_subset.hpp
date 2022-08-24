#ifndef AUTOMATA_STATE_SUBSET_HPP
#define AUTOMATA_STATE_SUBSET_HPP

#include <vector>

#include "automata/nfa/state.hpp"

namespace rematch {

struct StateSubset {
  StateSubset(size_t nfa_size) : bitset(nfa_size, false) {}

  void add(LogicalVA::State* q) {
    if(!bitset[q->id]) {
      subset.push_back(q);
      bitset[q->id] = true;
    }
  }

  std::vector<LogicalVA::State*> subset;
  std::vector<bool> bitset;

}; // end struct StateSubset

} // end namespace rematch

#endif // AUTOMATA_STATE_SUBSET_HPP
