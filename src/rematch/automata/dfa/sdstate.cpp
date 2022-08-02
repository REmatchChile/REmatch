#include "sdstate.hpp"

#include <algorithm>

#include "automata/nfa/state.hpp"

namespace rematch {

int SDState::ID = 0;

SDState::SDState(size_t tot_states)
    : id_(ID++), states_bitmap_(tot_states, false) {}

SDState::SDState(size_t tot_states, std::vector<LogicalVA::State*> states)
    : id_(ID++), states_bitmap_(tot_states, false), states_subset_(states) {
  for (auto &p : states_subset_) {
    states_bitmap_[p->id] = true;
    if (p->accepting())
      set_accepting(true);
  }
}

SDState::SDState(size_t tot_states, std::set<LogicalVA::State *> states)
    : id_(ID++), states_bitmap_(tot_states, false),
      states_subset_(states.begin(), states.end()) {
  for (auto &p : states_subset_) {
    states_bitmap_[p->id] = true;
    if (p->accepting())
      set_accepting(true);
  }
}

void SDState::add_state(LogicalVA::State *p) {
  auto lower = std::lower_bound(
      states_subset_.begin(), states_subset_.end(), p,
      [](const LogicalVA::State *s1, const LogicalVA::State *s2) { return s1->id < s2->id; });

  if (lower == states_subset_.end() || (*lower)->id != p->id) {
    states_subset_.insert(lower, p);
    states_bitmap_[p->id] = true;
  }
}

} // end namespace rematch