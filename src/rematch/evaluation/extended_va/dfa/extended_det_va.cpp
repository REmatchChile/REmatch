#include "extended_det_va.hpp"
#include <iostream>
#include <random>

namespace REmatch {


ExtendedDetVA::ExtendedDetVA(ExtendedVA& extended_va,
                             uint_fast32_t max_deterministic_states)
    : extended_va_(extended_va),
      num_states(0),
      phase(-1),
      clock_pointer(0),
      dfa_states_checker_(max_deterministic_states)
       {
  create_initial_state();
}

ExtendedDetVA::~ExtendedDetVA() {
  for (size_t i = 0; i < num_states; ++i) {
    delete states[i];
  }
  num_states = 0;
}

void ExtendedDetVA::create_initial_state() {
  StatesPtrSet initial_subset;

  initial_subset.insert(extended_va_.initial_state());
  initial_state_ = new ExtendedDetVAState(initial_subset);
  initial_state_->set_initial(true);
  if (num_states < REmatch::DEFAULT_MAX_DETERMINISTIC_STATES) {
    states[num_states++] = initial_state_;
  } else {
    throw std::runtime_error("Se ha alcanzado el límite de estados determinísticos");
  }
  
  dfa_states_checker_.count_state();
  StatesBitset initial_state_bitset = get_bitset_from_states_set(initial_subset);
  bitset_to_state_map[initial_state_bitset] = initial_state_;

}

std::vector<CaptureSubsetPair> ExtendedDetVA::get_next_states(
    ExtendedDetVAState*& current_state, char letter) {
  
  set_phase(current_state->phase);
  current_state->second_chance = true;
  std::optional<std::vector<CaptureSubsetPair>> capture_subset_pairs =
      current_state->get_transition(letter);

  // Mejora: ir a buscar solo el estado faltante
  if (capture_subset_pairs) {
    for (auto& capture_subset_pair : capture_subset_pairs.value()) {
      if (capture_subset_pair.subset->id != capture_subset_pair.id) {
      goto end; 
    }}
    return capture_subset_pairs.value();}
  end:


  std::unordered_map<std::bitset<64>, StatesPtrSet>
      captures_subset_map = get_map_with_next_subsets(current_state, letter);

  capture_subset_pairs = add_transitions_to_vector(captures_subset_map);

  current_state->cache_transition(letter, capture_subset_pairs);

  return capture_subset_pairs.value();
}

std::unordered_map<std::bitset<64>, StatesPtrSet>
ExtendedDetVA::get_map_with_next_subsets(ExtendedDetVAState*& current_state, char letter) {
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

std::vector<CaptureSubsetPair> ExtendedDetVA::add_transitions_to_vector(
    std::unordered_map<std::bitset<64>, StatesPtrSet>& captures_subset_map) {

  std::vector<CaptureSubsetPair> capture_subset_pairs;
  std::bitset<64> empty_capture(0);

  auto it = captures_subset_map.find(empty_capture);
  if (it != captures_subset_map.end()) {
    ExtendedDetVAState* next_state = get_state_from_subset(it->second);
    CaptureSubsetPair capture_subset_pair = CaptureSubsetPair{empty_capture, next_state, next_state->id};
    capture_subset_pairs.push_back(capture_subset_pair);

    captures_subset_map.erase(it);
  }

  for (auto& bitset_subset_pair : captures_subset_map) {
    ExtendedDetVAState* next_state = get_state_from_subset(bitset_subset_pair.second);
    auto capture_subset_pair = CaptureSubsetPair{bitset_subset_pair.first, next_state, next_state->id};
    capture_subset_pairs.push_back(capture_subset_pair);
  }

  return capture_subset_pairs;
}

ExtendedDetVAState* ExtendedDetVA::get_state_from_subset(StatesPtrSet &states_set) {
  StatesBitset states_bitset = get_bitset_from_states_set(states_set);

  if (bitset_to_state_map.count(states_bitset))
    return bitset_to_state_map[states_bitset];

  ExtendedDetVAState* new_state = create_state(states_set, states_bitset);
  return new_state;
}

ExtendedDetVAState* ExtendedDetVA::create_state(StatesPtrSet &states_set) {
  StatesBitset states_bitset = get_bitset_from_states_set(states_set);
  return create_state(states_set, states_bitset);
}

ExtendedDetVAState* ExtendedDetVA::create_state(StatesPtrSet &states_set, StatesBitset states_bitset) {
  
  ExtendedDetVAState* new_state;

  if (num_states < REmatch::DEFAULT_MAX_DETERMINISTIC_STATES) {
    new_state = new ExtendedDetVAState(states_set);
    bitset_to_state_map[states_bitset] = new_state;
    states[num_states++] = new_state;
    dfa_states_checker_.count_state();
  } else {

    ExtendedDetVAState* old_state = get_state_to_replace();

    std::vector<ExtendedVAState*> old_subset_vector = old_state->get_states_subset();
    StatesPtrSet old_subset_set(old_subset_vector.begin(), old_subset_vector.end());
    StatesBitset old_bitset = get_bitset_from_states_set(old_subset_set);
    bitset_to_state_map.erase(old_bitset);

    old_state->reset(states_set);  
    bitset_to_state_map[states_bitset] = old_state;

    new_state = old_state;
  }
  
  return new_state;
}

StatesBitset ExtendedDetVA::get_bitset_from_states_set(
    StatesPtrSet& states_subset) {

  StatesBitset states_bitset(extended_va_.size());

  for (auto& state : states_subset) {
    states_bitset[state->id] = true;
  }

  return states_bitset;
}

void ExtendedDetVA::set_state_initial_phases() {
  for (size_t i = 0; i < num_states; ++i) {
        states[i]->set_phase(-1);
    }
}

size_t ExtendedDetVA::get_extended_va_size() {
  return extended_va_.size();
}

void ExtendedDetVA::set_phase(int new_phase) {
    phase = new_phase;
}

int ExtendedDetVA::get_phase() const {
    return phase;
}

void ExtendedDetVA::set_clock_pointer(size_t new_clock_pointer) {
    clock_pointer = new_clock_pointer;
}

ExtendedDetVAState* ExtendedDetVA::get_state_to_replace() {
    // Si da dos vueltas, se duplica el tamaño del buffer states
    while (true) {

        clock_pointer = (clock_pointer + 1) % DEFAULT_MAX_DETERMINISTIC_STATES;

        auto* candidate_state = states[clock_pointer];

        if (candidate_state->phase == phase || candidate_state->phase == phase - 1 || candidate_state->is_initial()) {
            continue;
        }

        if (candidate_state->second_chance) {
            candidate_state->second_chance = false;
            continue;
        }

        return candidate_state;
    }
}


}  // namespace REmatch

