#ifndef SRC_REMATCH_AUTOMATA_NFA_SVA_HPP
#define SRC_REMATCH_AUTOMATA_NFA_SVA_HPP

#include <vector>

#include "filtering_module/search_variable_set_automaton/nfa/search_nfa_state.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"

namespace rematch {

// A non-deterministic automaton optimized for output fast searching.
class SearchNFA {
 public:

 private:
  std::vector<SearchNFAState*> states;
  SearchNFAState* initial_state_;
  SearchNFAState* accepting_state_;

 public:
  SearchNFA() = delete;
  SearchNFA(LogicalVA const &A);

  std::vector<SearchNFAState*> const & get_states() const;

  SearchNFAState* initial_state() const;
  SearchNFAState* accepting_state() const;

  void relabel_states();

  size_t size() const { return states.size(); }

 private:

}; // end class SearchNFA

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_SVA_HPP
