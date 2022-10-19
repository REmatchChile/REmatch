#ifndef SRC_REMATCH_AUTOMATA_NFA_SEARCH_STATE_HPP
#define SRC_REMATCH_AUTOMATA_NFA_SEARCH_STATE_HPP

#include <vector>

#include "automata/nfa/search/searchva.hpp"

namespace rematch {

class SearchVA::State : public AbsState {
public:
  std::vector<SearchVA::Transition *> transitions() const {
    return transitions_;
  }

  std::vector<SearchVA::Transition *> next_transitions(unsigned int code) const;

private:
  std::vector<SearchVA::Transition *> transitions_;
}; // end SearchVA::State

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_SEARCH_STATE_HPP
