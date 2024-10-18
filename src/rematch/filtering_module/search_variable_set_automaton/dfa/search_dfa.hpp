#ifndef AUTOMATA_DFA_SDFA_HPP
#define AUTOMATA_DFA_SDFA_HPP

#include <string>
#include <vector>
#include <memory>
#include <bitset>
#include <unordered_map>

#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa_state.hpp"
#include "filtering_module/search_variable_set_automaton/nfa/search_nfa.hpp"
#include "exceptions/dfa_state_limit_checker.hpp"

namespace REmatch {
inline namespace filtering_module {

using DFAStatesTableBools = std::unordered_map<std::vector<bool>, SearchDFAState*>;

/**
 * A searchDFA simulates a deterministic SearchNFA automaton
 */
class SearchDFA {

 public:
  std::vector<SearchDFAState*> states;

 private:
  SearchDFAState* current_state;
  SearchDFAState* initial_state;
  std::vector<SearchNFAState*> initial_nfa_states;

  SearchNFA sVA_;
  std::unordered_map<std::vector<bool>, SearchDFAState*> dfa_state_catalog;
  DFAStateLimitChecker dfa_states_checker_;

 public:
  SearchDFA(LogicalVA const& logical_va,
            DFAStateLimitChecker dfa_states_checker = DFAStateLimitChecker(1000));
  ~SearchDFA();

  SearchDFAState* get_initial_state() {return initial_state;}

  std::string pprint();

  size_t size() const {return states.size();}

  /**
   * next_state is the workhorse that allows DFASimulation, it returns
   * precomputed DFAState's or computes them and stores the result.
   */
  SearchDFAState* next_state(char a);
  void reset() { current_state = initial_state;}

  size_t get_search_nfa_size();

 private:

  SearchDFAState* create_initial_dfa_state();
  void visit_states(
    std::vector<SearchNFAState*> const &states_subset,
    std::set<SearchNFAState*> &subset,
    std::vector<bool> &subsetBitset,
    char &a);
};

}
}

#endif
