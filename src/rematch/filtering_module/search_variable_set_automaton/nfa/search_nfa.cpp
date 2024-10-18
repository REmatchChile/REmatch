#include "filtering_module/search_variable_set_automaton/nfa/search_nfa.hpp"

#include <iostream>
#include <map>

#include "parsing/charclass.hpp"
#include "evaluation/start_end_chars.hpp"
#ifdef TRACY_ENABLE
#include "tracy/Tracy.hpp"
#endif

namespace REmatch {
inline namespace filtering_module {

SearchNFA::SearchNFA(LogicalVA const &A) {
  #ifdef TRACY_ENABLE
  ZoneScoped;
  #endif

  LogicalVA A_prim(A);

  A_prim.remove_captures();

  A_prim.remove_epsilon();

  A_prim.trim();

  A_prim.relabel_states();

  std::map<unsigned int, SearchNFAState*> logical_va_state_id_to_search_nfa_state;
  for (LogicalVAState *logical_va_state : A_prim.states) {
    states.push_back(new SearchNFAState(logical_va_state));
    logical_va_state_id_to_search_nfa_state
      [logical_va_state->id] = states.back();
  }

  for (LogicalVAState *logical_va_state : A_prim.states) {
    SearchNFAState *from_state =
      logical_va_state_id_to_search_nfa_state[logical_va_state->id];
    for (LogicalVAFilter *logical_va_filter : logical_va_state->filters) {
      CharClass charclass = logical_va_filter->charclass;
      SearchNFAState *next =
        logical_va_state_id_to_search_nfa_state
          [logical_va_filter->next->id];
      from_state->add_filter(charclass, next);
    }

    for (LogicalVAAnchor* logical_va_anchor : logical_va_state->anchors) {
      CharClass charclass = logical_va_anchor->is_start()
                                ? CharClass(START_CHAR)
                                : CharClass(END_CHAR);
      SearchNFAState* next =
          logical_va_state_id_to_search_nfa_state[logical_va_anchor->next->id];
      from_state->add_filter(charclass, next);
    }
  }

  initial_state_ =
    logical_va_state_id_to_search_nfa_state[A_prim.initial_state()->id];
  accepting_state_ =
    logical_va_state_id_to_search_nfa_state[A_prim.accepting_state()->id];
  relabel_states();
}

SearchNFA::~SearchNFA() {
  for (auto& state : states) {
    delete state;
  }
}

void SearchNFA::relabel_states() {
  for(auto& state: states) {
    state->tempMark = false;
  }

  std::deque<SearchNFAState*> stack;

  int current_id = 0;

  stack.push_back(initial_state_);
  initial_state_->tempMark = true;

  while(!stack.empty()) {
    SearchNFAState* current = stack.back(); stack.pop_back();

    current->id = current_id++;
    for(auto &filt: current->filters) {
      if(!filt->next->tempMark) {
        stack.push_back(filt->next);
        filt->next->tempMark = true;
      }
    }
  }
}

std::vector<SearchNFAState*> const & SearchNFA::get_states() const { return states; }

SearchNFAState* SearchNFA::initial_state() const { return initial_state_; }

SearchNFAState* SearchNFA::accepting_state() const { return accepting_state_; }

}
}
