#include "extended_det_va.hpp"

#include <iostream>
#include "state_manager/clock_state_manager.hpp"
#include "state_manager/default_state_manager.hpp"

namespace REmatch {

ExtendedDetVA::ExtendedDetVA(ExtendedVA& extended_va, Flags flags,
                             uint_fast32_t max_deterministic_states)
    : extended_va_(extended_va) {
  if ((flags & Flags::CLOCK_POLICY) != Flags::NONE) {
    state_manager = std::make_unique<ClockStateManager>(extended_va.size());
  } else {
    state_manager =
        std::make_unique<DefaultStateManager>(extended_va.size(), max_deterministic_states);
  }
  state_manager->create_initial_state(extended_va.initial_state());
}

std::vector<CaptureSubsetPair> ExtendedDetVA::get_next_states(ExtendedDetVAState* current_state,
                                                              char letter) {
  set_phase(current_state->phase);
  // TODO: do i need to set second chance here?
  current_state->second_chance = true;
  std::optional<std::vector<CaptureSubsetPair>> capture_subset_pairs =
      current_state->get_transition(letter);

  // Mejora: ir a buscar solo el estado faltante
  if (capture_subset_pairs) {
    if (std::all_of(capture_subset_pairs.value().begin(), capture_subset_pairs.value().end(),
                    [](auto& pair) { return pair.subset->id == pair.id; })) {
      return capture_subset_pairs.value();
    }
  }

  std::unordered_map<std::bitset<64>, StatesPtrSet> captures_subset_map =
      get_next_subsets_map(current_state, letter);

  capture_subset_pairs = convert_to_dfa_states(captures_subset_map);

  current_state->cache_transition(letter, capture_subset_pairs);

  return capture_subset_pairs.value();
}

std::unordered_map<std::bitset<64>, StatesPtrSet> ExtendedDetVA::get_next_subsets_map(
    ExtendedDetVAState*& current_state, char letter) {
  std::unordered_map<std::bitset<64>, StatesPtrSet> captures_subset_map;

  for (auto& state_in_current_subset : current_state->get_states_subset()) {
    for (auto& read_capture : state_in_current_subset->read_captures) {
      if (read_capture->charclass.contains(letter)) {
        std::bitset<64> code = read_capture->captures_set;
        captures_subset_map[code].insert(read_capture->next);
      }
    }
  }
  return captures_subset_map;
}

std::vector<CaptureSubsetPair> ExtendedDetVA::convert_to_dfa_states(
    std::unordered_map<std::bitset<64>, StatesPtrSet>& captures_subset_map) {

  std::vector<CaptureSubsetPair> capture_subset_pairs;
  std::bitset<64> empty_capture(0);

  auto it = captures_subset_map.find(empty_capture);
  if (it != captures_subset_map.end()) {
    ExtendedDetVAState* next_state = state_manager->get_state_from_subset(it->second);
    auto capture_subset_pair = CaptureSubsetPair{empty_capture, next_state, next_state->id};
    capture_subset_pairs.push_back(capture_subset_pair);

    captures_subset_map.erase(it);
  }

  for (auto& bitset_subset_pair : captures_subset_map) {
    ExtendedDetVAState* next_state =
        state_manager->get_state_from_subset(bitset_subset_pair.second);
    auto capture_subset_pair =
        CaptureSubsetPair{bitset_subset_pair.first, next_state, next_state->id};
    capture_subset_pairs.push_back(capture_subset_pair);
  }

  return capture_subset_pairs;
}

void ExtendedDetVA::set_state_initial_phases() {
  state_manager->set_state_initial_phases();
}

size_t ExtendedDetVA::get_extended_va_size() {
  return extended_va_.size();
}

void ExtendedDetVA::set_phase(int32_t new_phase) {
  phase = new_phase;
}

int ExtendedDetVA::get_phase() const {
  return phase;
}

}  // namespace REmatch
