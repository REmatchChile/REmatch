#include "dstate.hpp"

#include <algorithm>

#include "automata/nfa/state.hpp"

namespace rematch {

DState::DState(size_t tot_states) : states_bitmap_(tot_states, false) {}

void DState::add_state(State* p) {
  auto lower = std::lower_bound(states_subset_.begin(), states_subset_.end(), p,
    [](const State* s1, const State* s2) { return s1->id < s2->id; }
  );

  if(lower == states_subset_.end() || (*lower)->id != p->id) {
    states_subset_.insert(lower, p);
    states_bitmap_[p->id] = true;
  }
}

} // end namespace rematch