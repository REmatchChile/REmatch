#ifndef EXTENDED_DET_VA_HPP
#define EXTENDED_DET_VA_HPP

#include "capture_subset_pair.hpp"
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "extended_det_va_state.hpp"

using StatesBitset = std::vector<bool>;

namespace rematch {

class ExtendedDetVA {
 private:

  ExtendedDetVAState* initial_state_;
  ExtendedVA extended_va_;
  std::unordered_map<StatesBitset, ExtendedDetVAState*>
      bitset_to_state_map;

  void create_initial_state();

  std::unordered_map<std::bitset<64>, std::set<ExtendedVAState*>>
  get_map_with_next_subsets(ExtendedDetVAState* &current_state, char letter);

  void add_transitions_with_empty_captures(
      std::unordered_map<std::bitset<64>, std::set<ExtendedVAState*>>&
          captures_subset_map,
      std::vector<CaptureSubsetPair*>& capture_subset_pairs);

  void add_transitions_with_captures(
      std::unordered_map<std::bitset<64>, std::set<ExtendedVAState*>>&
          captures_subset_map,
      std::vector<CaptureSubsetPair*>& capture_subset_pairs);

  ExtendedDetVAState* create_state(std::set<ExtendedVAState*> &states_set);

 public:
  ExtendedDetVA(ExtendedVA& extended_va);

  std::vector<ExtendedDetVAState*> states;

  std::vector<CaptureSubsetPair*> get_next_states(
      ExtendedDetVAState*& current_state, char letter);

  ExtendedDetVAState* get_initial_state() { return initial_state_; }

  StatesBitset get_bitset_from_states_set(
      std::set<ExtendedVAState*>& states_subset);

  ExtendedDetVAState* get_state_from_subset(
      std::set<ExtendedVAState*> &states_set);
};

}  // namespace rematch

#endif
