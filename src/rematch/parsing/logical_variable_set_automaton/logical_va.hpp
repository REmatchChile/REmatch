#pragma once

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cwchar>
#include <deque>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parsing/logical_variable_set_automaton/logical_va_state.hpp"

// TODO: Rearrange all variables.
// TODO: Change representation of states to a list of the form:
//             (initial, type of transition, final)
// TODO: Test backward transitions if the type of transition is not changed.

namespace REmatch {
class ExtendedVA;
inline namespace parsing {

class LogicalVA {
  /**
   * A Logical Variable Set Automaton is an automaton extended with capture
   * variables, equivalently expressive as the input REGEX.
   *
   * LogicalVA operations, all modify the current LogicalVA to get the result
   * (the operations are inplace)
   *
   * For the theoretical background refer to the paper:
   * REmatch: a regex engine for finding all matches
   * (Riveros, Van Sint Jan, Vrgoc 2023).
   */
  friend class ExtendedVA;

 private:

  LogicalVAState* new_state();

  LogicalVAState* init_state_;
  LogicalVAState* accepting_state_;

  bool accepts_epsilon_ = false;

 public:
  LogicalVAState* initial_state() const { return init_state_; }
  LogicalVAState* accepting_state() const { return accepting_state_; }

  std::vector<LogicalVAState*> states;

  /// Empty LogicalVA construction (only one LogicalVAState)
  LogicalVA();

  LogicalVA(const LogicalVA &A);

  /// Atomic VA
  LogicalVA(CharClass charclass);

  LogicalVA(LogicalVA&& logical_va);

  ~LogicalVA();

  LogicalVA& operator=(LogicalVA&& other);

  /**
   * Transforms the automaton graph to a trimmed automaton. This being that every
   * state is reachable from the initial state, and the final state is reachable
   * from every state.
   */
  void trim();

  /// Inplace transformation from R to RR'
  void cat(LogicalVA &a2);
  /// Equivalent to R1|R2
  void alter(LogicalVA &a2);
  /// Equivalent to R*
  void kleene();
  /// Equivalent to R+
  void strict_kleene();
  /// Equivalent to R?
  void optional();
  /// Equivalent to !x{R}
  void assign(std::bitset<64> open_code, std::bitset<64> close_code);
  /// Inplace transformation from R to R{min,max}
  void repeat(int min, int max);
  /// Remove capture transitions as if they were instantaneous (epsilon labeled)
  void remove_captures();
  /// Removes epsilon transitions.
  void remove_epsilon();
  /// Make is so that the node id's start in 0 and are increasing.
  void relabel_states();

  bool has_epsilon() const { return accepts_epsilon_; }

  void add_anchor(bool is_start);

  void remove_useless_anchors();

  bool has_useful_anchors();

  bool is_accepting_state_reachable();

  void set_accepting_state(LogicalVAState* state);
  void set_initial_state(LogicalVAState* state);

  friend std::ostream& operator<<(std::ostream& os, LogicalVA const &A);
private:
  void copy_states(
      const std::vector<LogicalVAState*> &old_states,
      std::unordered_map<LogicalVAState*, LogicalVAState*>
        &old_state_to_new_state_mapping);
  void copy_transitions(
      const std::vector<LogicalVAState*> &old_states,
      std::unordered_map<LogicalVAState*, LogicalVAState*>
        &old_state_to_new_state_mapping);

};

}
}
