
#include "extended_det_va_state.hpp"

namespace REmatch {

ExtendedDetVAState::ExtendedDetVAState() {
  id = ID++;
}

ExtendedDetVAState::ExtendedDetVAState(StatesPtrSet& states_subset)
    : states_subset_(states_subset.begin(), states_subset.end()) {
  id = ID++;

  for (auto& state : states_subset_) {
    if (state->is_accepting()) {
      is_accepting_ = true;
    }
  }
}

void ExtendedDetVAState::set_initial(bool initial) {
  is_initial_ = initial;
}

bool ExtendedDetVAState::is_initial() const {
  return is_initial_;
}

bool ExtendedDetVAState::is_accepting() const {
  return is_accepting_;
}

void ExtendedDetVAState::set_node(ECSNode* node) {
  output_node = node;
}

void ExtendedDetVAState::unset_node() {
  output_node = nullptr;
}

ECSNode* ExtendedDetVAState::get_node() const {
  return output_node;
}

void ExtendedDetVAState::set_phase(int phase) {
  this->phase = phase;
}

void ExtendedDetVAState::cache_transition(
    char letter, std::vector<CaptureSubsetPair>&& capture_subset_pairs) {
  cached_transitions[reinterpret_cast<uint8_t&>(letter)] =
      std::make_unique<std::vector<CaptureSubsetPair>>(capture_subset_pairs);
}

std::vector<CaptureSubsetPair>* ExtendedDetVAState::get_transition(
    char letter) {
  return cached_transitions[reinterpret_cast<uint8_t&>(letter)].get();
}

unsigned int ExtendedDetVAState::ID = 0;

}  // namespace REmatch
