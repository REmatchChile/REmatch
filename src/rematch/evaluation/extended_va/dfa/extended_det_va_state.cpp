
#include "extended_det_va_state.hpp"

namespace rematch {

ExtendedDetVAState::ExtendedDetVAState() {
  id = ID++;
  auto& memory_tracker = MemoryTracker::get_instance();
  memory_tracker.track(*this);
}

ExtendedDetVAState::ExtendedDetVAState(
    StatesPtrSet &states_subset)
    : states_subset_(states_subset.begin(), states_subset.end()) {
  id = ID++;

  for (auto& state: states_subset_) {
    if (state->is_accepting()) {
      is_accepting_ = true;
    }
  }

  auto& memory_tracker = MemoryTracker::get_instance();
  memory_tracker.track(*this);
}

ExtendedDetVAState::~ExtendedDetVAState() {
  auto& memory_tracker = MemoryTracker::get_instance();
  memory_tracker.untrack(*this);
  for (auto& optional_transition : cached_transitions) {
    if (optional_transition) {
      memory_tracker.untrack(optional_transition.value());
    }
  }
}

void ExtendedDetVAState::set_initial(bool initial) {
  is_initial_ = initial;
}

bool ExtendedDetVAState::is_initial() {
  return is_initial_;
}

bool ExtendedDetVAState::is_accepting() {
  return is_accepting_;
}

void ExtendedDetVAState::set_node(ECSNode* node) {
  output_node = node;
}

void ExtendedDetVAState::unset_node() {
  output_node = nullptr;
}

ECSNode* ExtendedDetVAState::get_node() {
  return output_node;
}

void ExtendedDetVAState::set_phase(int phase) {
  this->phase = phase;
}

void ExtendedDetVAState::cache_transition(
    char letter,
    std::optional<std::vector<CaptureSubsetPair*>> capture_subset_pairs) {
  cached_transitions[(uint8_t)letter] = capture_subset_pairs;
  MemoryTracker::get_instance().track(capture_subset_pairs.value());
}

std::optional<std::vector<CaptureSubsetPair*>> ExtendedDetVAState::get_transition(char letter) {
  return cached_transitions[(uint8_t)letter];
}

unsigned int ExtendedDetVAState::ID = 0;

}  // namespace rematch
