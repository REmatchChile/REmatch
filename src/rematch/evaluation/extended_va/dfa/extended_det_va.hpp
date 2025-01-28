#pragma once

#include <REmatch/constants.hpp>
#include "REmatch/flags.hpp"
#include "aliases.hpp"
#include "capture_subset_pair.hpp"
#include "evaluation/extended_va/dfa/state_manager/state_manager.hpp"
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "exceptions/dfa_state_limit_checker.hpp"
#include "extended_det_va_state.hpp"

namespace REmatch {

class ExtendedDetVA {
 private:
  ExtendedVA& extended_va_;
  std::unique_ptr<StateManager> state_manager;

  std::unordered_map<std::bitset<64>, StatesPtrSet> get_next_subsets_map(
      ExtendedDetVAState*& current_state, char letter);

  std::vector<CaptureSubsetPair> convert_to_dfa_states(
      std::unordered_map<std::bitset<64>, StatesPtrSet>& captures_subset_map);

  ExtendedDetVAState* create_state(StatesPtrSet& states_set);
  ExtendedDetVAState* create_state(StatesPtrSet& states_set, StatesBitset states_bitset);

  int32_t phase = -1;

 public:
  explicit ExtendedDetVA(ExtendedVA& extended_va, Flags flags = Flags() ,uint_fast32_t max_deterministic_states =
                                                      REmatch::DEFAULT_MAX_DETERMINISTIC_STATES);

  std::vector<CaptureSubsetPair> get_next_states(ExtendedDetVAState* current_state, char letter);

  ExtendedDetVAState* get_initial_state() { return state_manager->get_initial_state(); }

  void set_state_initial_phases();

  size_t get_extended_va_size();

  size_t get_num_states() const { return state_manager->get_num_states(); }

  void set_phase(int new_phase);

  int get_phase() const;

  void set_clock_pointer(size_t new_clock_pointer);

  ExtendedDetVAState* get_state_to_replace();
};

}  // namespace REmatch
