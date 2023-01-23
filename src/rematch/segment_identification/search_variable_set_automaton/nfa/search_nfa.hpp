#ifndef SRC_REMATCH_AUTOMATA_NFA_SVA_HPP
#define SRC_REMATCH_AUTOMATA_NFA_SVA_HPP

#include <vector>

#include "segment_identification/search_variable_set_automaton/nfa/search_nfa_state.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"

namespace rematch {

// A non-deterministic automaton optimized for output fast searching.
class SearchVA {
 public:

 private:
  std::vector<SearchNFAState*> states;
  SearchNFAState* initial_state_;
  SearchNFAState* accepting_state_;

 public:
  SearchVA() = delete;
  SearchVA(LogicalVA const &A);

  std::vector<SearchNFAState*> const & get_states() const;

  SearchNFAState* initial_state() const;
  SearchNFAState* accepting_state() const;

  bool has_epsilon() const;

  size_t size() const { return states.size(); }

 private:

}; // end class SearchVA

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_SVA_HPP
