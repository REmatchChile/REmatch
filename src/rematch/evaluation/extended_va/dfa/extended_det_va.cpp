
#include "extended_det_va.hpp"

namespace rematch {

ExtendedDetVA::ExtendedDetVA(ExtendedVA &extended_va)
    : extended_va_(extended_va) {
  create_initial_state();
}

void ExtendedDetVA::create_initial_state() {
  std::set<ExtendedVAState*> initial_subset;
  initial_subset.insert(extended_va_.initial_state());

  initial_state_ = new ExtendedDetVAState(initial_subset);
  initial_state_->set_initial(true);
  states.push_back(initial_state_);

  StatesBitset initial_state_bitset = get_bitset_from_states_set(initial_subset);
  bitset_to_state_map[initial_state_bitset] = initial_state_;
}

std::vector<CaptureSubsetPair*> ExtendedDetVA::get_next_states(
    ExtendedDetVAState*& current_state, char letter) {

  if (current_state->contains_cached_transition(letter))
    return current_state->get_transition(letter);

  std::unordered_map<std::bitset<64>, std::set<ExtendedVAState*>> captures_subset_map;

  captures_subset_map = get_map_with_next_subsets(current_state, letter);

  std::vector<CaptureSubsetPair*> capture_subset_pairs;

  // add transitions without captures first
  add_transitions_with_empty_captures(captures_subset_map, capture_subset_pairs);
  add_transitions_with_captures(captures_subset_map, capture_subset_pairs);

  current_state->cached_transitions[letter] = capture_subset_pairs;

  return capture_subset_pairs;
}

std::unordered_map<std::bitset<64>, std::set<ExtendedVAState*>>
ExtendedDetVA::get_map_with_next_subsets(ExtendedDetVAState*& current_state, char letter) {
  
  std::unordered_map<std::bitset<64>, std::set<ExtendedVAState*>> captures_subset_map;

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

void ExtendedDetVA::add_transitions_with_empty_captures(
    std::unordered_map<std::bitset<64>, std::set<ExtendedVAState*>>& captures_subset_map,
    std::vector<CaptureSubsetPair*>& capture_subset_pairs) {

  std::bitset<64> no_captures_set;

  if (captures_subset_map.count(no_captures_set)) {
    ExtendedDetVAState* next_state = get_state_from_subset(captures_subset_map[no_captures_set]);
    CaptureSubsetPair* capture_subset_pair = new CaptureSubsetPair{no_captures_set, next_state};
    capture_subset_pairs.push_back(capture_subset_pair);

    auto it = captures_subset_map.find(no_captures_set);
    captures_subset_map.erase(it);
  }
}

void ExtendedDetVA::add_transitions_with_captures(
    std::unordered_map<std::bitset<64>, std::set<ExtendedVAState*>>& captures_subset_map,
    std::vector<CaptureSubsetPair*>& capture_subset_pairs) {

  for (auto& bitset_subset_pair : captures_subset_map) {
    ExtendedDetVAState* next_state = get_state_from_subset(bitset_subset_pair.second);
    auto capture_subset_pair = new CaptureSubsetPair{bitset_subset_pair.first, next_state};
    capture_subset_pairs.push_back(capture_subset_pair);
  }
}

ExtendedDetVAState* ExtendedDetVA::get_state_from_subset(std::set<ExtendedVAState*> &states_set) {
  StatesBitset states_bitset = get_bitset_from_states_set(states_set);

  if (bitset_to_state_map.count(states_bitset))
    return bitset_to_state_map[states_bitset];

  ExtendedDetVAState* new_state = create_state(states_set);
  return new_state;
}

ExtendedDetVAState* ExtendedDetVA::create_state(std::set<ExtendedVAState*> &states_set) {
  ExtendedDetVAState* new_state = new ExtendedDetVAState(states_set);
  StatesBitset states_bitset = get_bitset_from_states_set(states_set);
  bitset_to_state_map[states_bitset] = new_state;
  states.push_back(new_state);
  return new_state;
}

StatesBitset ExtendedDetVA::get_bitset_from_states_set(
    std::set<ExtendedVAState*>& states_subset) {

  StatesBitset states_bitset(extended_va_.size());

  for (auto& state : states_subset) {
    states_bitset[state->id] = true;
  }

  return states_bitset;
}

}  // namespace rematch
