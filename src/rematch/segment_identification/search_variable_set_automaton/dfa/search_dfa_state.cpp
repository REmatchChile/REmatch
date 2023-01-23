#include "segment_identification/search_variable_set_automaton/dfa/search_dfa_state.hpp"
#include <algorithm>


namespace rematch {

int SearchDFAState::ID = 0;

SearchDFAState::SearchDFAState(size_t tot_states)
  : id_(ID++),
    transitions_(256),
    states_bitmap_(tot_states, false) {}

SearchDFAState::SearchDFAState(size_t tot_states, std::vector<SearchNFAState*> states)
    : id_(ID++),
      transitions_(256),
      states_bitmap_(tot_states, false),
      states_subset_(states) {
  for(auto &p: states_subset_) {
    states_bitmap_[p->id] = true;
    if(p->accepting())
      flags |= kAcceptingSearchNFAState;
  }
}

SearchDFAState::SearchDFAState(size_t tot_states, std::set<SearchNFAState*> states)
    : id_(ID++),
      transitions_(256),
      states_bitmap_(tot_states, false),
      states_subset_(states.begin(), states.end()) {
  for(auto &p: states_subset_) {
    states_bitmap_[p->id] = true;
    if(p->accepting())
      flags |= kAcceptingSearchNFAState;
  }
}

void SearchDFAState::add_state(SearchNFAState* p) {
  auto lower = std::lower_bound(states_subset_.begin(), states_subset_.end(), p,
    [](const SearchNFAState* s1, const SearchNFAState* s2) { return s1->id < s2->id; }
  );

  if(lower == states_subset_.end() || (*lower)->id != p->id) {
    states_subset_.insert(lower, p);
    states_bitmap_[p->id] = true;
  }
}

void SearchDFAState::add_direct(char a, SearchDFAState* q) {
  if(transitions_[a] == nullptr)
    transitions_[a] = std::make_unique<Transition>(q);
  else
    transitions_[a]->add_direct(q);
}

void SearchDFAState::add_capture(char a, std::bitset<64> S, SearchDFAState* q) {
  if(transitions_[a] == nullptr)
    transitions_[a] = std::make_unique<Transition>(new DCapture(S, q));
  else
    transitions_[a]->add_capture(new DCapture(S, q));
}

void SearchDFAState::add_empty(char a) {
  transitions_[a] = std::make_unique<Transition>();
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

void SearchDFAState::pass_node(ECSNode* n) {
  // TODO: Input the ECS too.
  //node = n;
  //++n->ref_count_;
}

} // end namespace rematch
