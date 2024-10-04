#ifndef EXTENDED_VA_HPP
#define EXTENDED_VA_HPP

#include <map>
#include <queue>
#include "extended_va_state.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "parsing/parser.hpp"
#include "extended_va_assert.hpp"
#include "evaluation/start_end_chars.hpp"

namespace REmatch {

bool check_if_capture_state_is_useless(ExtendedVAState* state);

class ExtendedVA {

public:
  std::vector<ExtendedVAState*> states;

  ExtendedVA(LogicalVA const &logical_va);

  ExtendedVA(ExtendedVA&& other) noexcept;

  ~ExtendedVA();

  ExtendedVAState* initial_state() const { return initial_state_; }
  void set_initial_state(ExtendedVAState *s);

  ExtendedVAState* accepting_state() const { return accepting_state_; }
  void set_accepting_state(ExtendedVAState *s);

  void clean_for_determinization();
  void duplicate();
  void add_loop_to_initial_state();
  void relabel_states();
  size_t size() { return states.size(); }

private:
  ExtendedVAState* initial_state_;
  ExtendedVAState* accepting_state_;

  void capture_closure();

  void copy_data_from_logical_va(LogicalVA &logical_va);
  void copy_transitions_from_logical_va(LogicalVA &logical_va, std::map<unsigned int, ExtendedVAState*> &id_map);
  void copy_states_from_logical_va(LogicalVA &logical_va, std::map<unsigned int, ExtendedVAState*> &id_map);

  std::queue<ExtendedVAState*> inv_topological_sort();
  void inv_topological_sort_util(ExtendedVAState* state, std::queue<ExtendedVAState*> *Q);

  void remove_useless_capture_states();

  void add_read_captures_transitions();
  void remove_filter_transitions();
  void remove_capture_transitions();

  void trim();

  ExtendedVAState* create_new_state();

  friend std::ostream& operator<<(std::ostream& os, ExtendedVA const &extended_va);
};

}

#endif