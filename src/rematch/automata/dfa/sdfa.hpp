#ifndef AUTOMATA_DFA_SDFA_HPP
#define AUTOMATA_DFA_SDFA_HPP

#include <bitset>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "automata/dfa/sdstate.hpp"
#include "automata/dfa/transition.hpp"

#include "automata/nfa/sva.hpp"
#include "regex/regex_options.hpp"

namespace rematch {

class VariableFactory;
class DetState;
class ExtendedVA;

using SubsetTable = std::unordered_map<std::vector<bool>, SDState *>;

class SearchDFA {
public:
  // Vector of pointers of States for resizing:
  std::vector<SDState*> states;
  std::vector<SDState*> final_states;

  std::vector<std::string> varNames;

  SearchDFA(SearchVA const &A);

  //  ---  Getters  ---  //

  SDState *initial_state() { return initial_state_; }

  // @brief Check if the empty word is inside the automaton's language
  bool has_epsilon() const { return has_epsilon_; }

  // @brief Number of states in the automaton's graph
  size_t size() const { return states.size(); }

  // ---  Determinization  ---  //

  SDState *next_state(SDState *q, char a);

private:
  SDState *new_dstate();

  // The starting state of the dfa
  SDState *initial_state_;

  bool has_epsilon_;

  SearchVA const &sVA_;

  SubsetTable dstates_table_;

  // Access to variable and filter factory
  std::shared_ptr<VariableFactory> vfactory_;
  std::shared_ptr<FilterFactory> ffactory_;
};

} // end namespace rematch

#endif // AUTOMATA_DFA_SDFA_HPP
