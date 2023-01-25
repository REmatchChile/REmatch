#include "segment_identification/search_variable_set_automaton/dfa/search_dfa_state.hpp"
#include <algorithm>
#include <iostream>


namespace rematch {

int SearchDFAState::ID = 0;

SearchDFAState::SearchDFAState(SearchNFAState *state)
  : transitions(256) ,
    id(ID++) {
      states_subset_.push_back(state);
    }

SearchDFAState::SearchDFAState(std::set<SearchNFAState*> &states)
    : transitions(256),
      id(ID++),
      states_subset_(states.begin(), states.end()) {
  for(auto &p: states) {
    if(p->accepting())
      set_accepting();
  }
  if (states_subset_.empty())
    set_ends();
}

void SearchDFAState::set_accepting() {
  flags |= Flags::kAcceptingSearchNFAState;
}

void SearchDFAState::set_initial() {
  flags |= Flags::kInitialSearchNFAState;
}

void SearchDFAState::set_ends() {
  flags |= Flags::kEndsNFAState;
}

} // end namespace rematch
