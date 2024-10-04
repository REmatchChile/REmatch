#include "filtering_module/search_variable_set_automaton/nfa/search_nfa_state.hpp"

#include <list>
#include <set>
#include <bitset>

namespace REmatch {
inline namespace filtering_module {

SearchNFAState::SearchNFAState(): flags(kDefaultSearchNFAState) { id = ID++; }

SearchNFAState::SearchNFAState(const SearchNFAState& s): flags(s.flags) { id = ID++; }

SearchNFAState::~SearchNFAState() {
  // We remove every transition coming out and coming into the state.
  for(auto &f: filters) {
    f->next->backward_filters_.remove(f);
    delete f;
  }
  for(auto &f: backward_filters_) {
    f->from->filters.remove(f);
    delete f;
  }
}

bool SearchNFAState::operator==(const SearchNFAState &rhs) const { return id == rhs.id;}

void SearchNFAState::set_accepting(bool b) {
  if(b)
    flags |= kFinalSearchNFAState;
  else
    flags &= ~kFinalSearchNFAState;

}

void SearchNFAState::set_initial(bool b) {
  if(b)
    flags |= kInitialSearchNFAState;
  else
    flags &= ~kInitialSearchNFAState;
}

void SearchNFAState::add_filter(CharClass charclass, SearchNFAState* next) {
  for(auto const& filter: this->filters)
    if(filter->charclass == charclass && filter->next == next) return;

  auto filter = new SearchNFAFilter(this, charclass, next);
  filters.push_back(filter);
  next->backward_filters_.push_back(filter);
}

unsigned int SearchNFAState::ID = 0;

}
}

