#ifndef EXTENDED_DET_VA_HPP
#define EXTENDED_DET_VA_HPP

#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "exceptions/complex_query_exception.hpp"
#include "extended_det_va_state.hpp"
#include "capture_subset_pair.hpp"
#include "exceptions/dfa_state_limit_checker.hpp"

namespace rematch {

class ExtendedDetVA {
 private:

  ExtendedDetVAState* initial_state_;
  ExtendedVA& extended_va_;
  std::unordered_map<StatesBitset, ExtendedDetVAState*>
      bitset_to_state_map;

  void create_initial_state();

  std::unordered_map<std::bitset<64>, StatesPtrSet> get_map_with_next_subsets(
      ExtendedDetVAState*& current_state, char letter);

  std::vector<CaptureSubsetPair> add_transitions_to_vector(
      std::unordered_map<std::bitset<64>, StatesPtrSet>& captures_subset_map);

  ExtendedDetVAState* create_state(StatesPtrSet &states_set);
  ExtendedDetVAState* create_state(StatesPtrSet &states_set, StatesBitset states_bitset);

 public:
  DFAStateLimitChecker dfa_states_checker_;
  ExtendedDetVA(ExtendedVA& extended_va, DFAStateLimitChecker dfa_states_checker = DFAStateLimitChecker());

  ~ExtendedDetVA();

  std::vector<ExtendedDetVAState*> states;

  std::vector<CaptureSubsetPair> get_next_states(
      ExtendedDetVAState*& current_state, char letter);

  ExtendedDetVAState* get_initial_state() { return initial_state_; }

  StatesBitset get_bitset_from_states_set(
      StatesPtrSet& states_subset);

  ExtendedDetVAState* get_state_from_subset(
      StatesPtrSet &states_set);

  void set_state_initial_phases();

  size_t get_extended_va_size();
};

}  // namespace rematch

#endif
