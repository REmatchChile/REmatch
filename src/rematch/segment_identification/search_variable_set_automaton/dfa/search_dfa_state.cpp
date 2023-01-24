#include "segment_identification/search_variable_set_automaton/dfa/search_dfa_state.hpp"
#include <algorithm>


namespace rematch {

int SearchDFAState::ID = 0;

SearchDFAState::SearchDFAState()
  : transitions(256) ,
    id(ID++) {}

SearchDFAState::SearchDFAState(std::vector<SearchNFAState*> states)
    : transitions(256),
      id(ID++),
      states_subset_(states) {
  for(auto &p: states_subset_) {
    if(p->accepting())
      flags |= kAcceptingSearchNFAState;
  }
}

SearchDFAState::SearchDFAState(std::set<SearchNFAState*> states)
    : transitions(256),
      id(ID++),
      states_subset_(states.begin(), states.end()) {
  for(auto &p: states_subset_) {
    if(p->accepting())
      flags |= kAcceptingSearchNFAState;
  }
}

void SearchDFAState::set_accepting(bool b) {
  if(b)
    flags |= Flags::kAcceptingSearchNFAState;
  else
    flags &= ~Flags::kAcceptingSearchNFAState;
}

void SearchDFAState::set_initial(bool b) {
  if(b)
    flags |= Flags::kInitialSearchNFAState;
  else
    flags &= ~Flags::kInitialSearchNFAState;
}

} // end namespace rematch
