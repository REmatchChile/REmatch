#include "segment_identification/search_variable_set_automaton/nfa/search_nfa_state.hpp"

#include <list>
#include <set>
#include <bitset>

namespace rematch {

SearchNFAState::SearchNFAState(): flags(kDefaultSearchNFAState) { id = ID++; }

SearchNFAState::SearchNFAState(const SearchNFAState& s): flags(s.flags) { id = ID++; }

SearchNFAState::~SearchNFAState() {
  // We remove every transition coming out and coming into the state.
  for(auto &f: filters)
    f->next->backward_filters_.remove(f);
  for(auto &f: backward_filters_)
    f->from->filters.remove(f);
  //for(auto &c: captures)
    //c->next->backward_captures_.remove(c);
  //for(auto &c: backward_captures_)
    //c->from->captures.remove(c);
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

//void SearchNFAState::add_capture(std::bitset<64> code, SearchNFAState* next) {
  //for(auto const &capture: this->captures) {
    //if (capture->code == code && capture->next == next)
      //return;
  //}

  //auto sp = std::make_shared<SearchNFACapture>(this, code, next);
  //captures.push_back(sp);
  //next->backward_captures_.push_back(sp);
//}

void SearchNFAState::add_filter(CharClass charclass, SearchNFAState* next) {
  for(auto const& filter: this->filters)
    if(filter->charclass == charclass && filter->next == next) return;

  auto filter = new SearchNFAFilter(this, charclass, next);
  filters.push_back(filter);
  next->backward_filters_.push_back(filter);
}

//void SearchNFAState::add_epsilon(SearchNFAState* q) {
  //for(auto const& epsilon: epsilons)
    //if(epsilon->next == q) return;

  //auto sp = std::make_shared<SearchNFAEpsilon>(this, q);
  //epsilons.push_back(sp);
  //q->backward_epsilons_.push_back(sp);
//}

unsigned int SearchNFAState::ID = 0;

} // end namespace rematch

