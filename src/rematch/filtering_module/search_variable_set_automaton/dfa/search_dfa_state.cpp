#include "filtering_module/search_variable_set_automaton/dfa/search_dfa_state.hpp"
#include <algorithm>
#include <iostream>


namespace REmatch {
inline namespace filtering_module {

int SearchDFAState::ID = 0;

SearchDFAState::SearchDFAState()
  : transitions(256) ,
    id(ID++) { set_ends(); }

SearchDFAState::SearchDFAState(std::set<SearchNFAState*> &states,
                                                    bool is_ends)
    : transitions(256),
      id(ID++),
      states_subset_(states.begin(), states.end()) {
  for(auto &p: states) {
    if(p->accepting())
      set_accepting();
  }
  if (is_ends)
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

}
}
