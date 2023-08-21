
#include "extended_det_va_state.hpp"

namespace rematch {

ExtendedDetVAState::ExtendedDetVAState() {
  id = ID++;
}

ExtendedDetVAState::ExtendedDetVAState(
    std::set<ExtendedVAState*> &states_subset)
    : states_subset_(states_subset.begin(), states_subset.end()) {
  id = ID++;
}

void ExtendedDetVAState::set_initial(bool initial) {
  is_initial_ = initial;
}

void ExtendedDetVAState::add_to_subset(ExtendedVAState* state) {
  states_subset_.push_back(state);
}

bool ExtendedDetVAState::contains_cached_transition(char letter) {
  return cached_transitions.count(letter);
}

std::vector<CaptureSubsetPair*> ExtendedDetVAState::get_transition(char letter) {
  return cached_transitions[letter];
}

bool ExtendedDetVAState::is_initial() {
  return is_initial_;
}

unsigned int ExtendedDetVAState::ID = 0;

}  // namespace rematch
