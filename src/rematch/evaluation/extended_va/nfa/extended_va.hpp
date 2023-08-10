#ifndef EXTENDED_VA_HPP
#define EXTENDED_VA_HPP

#include <map>
#include <queue>
#include "extended_va_state.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "parsing/parser.hpp"
#include "extended_va_assert.hpp"

namespace rematch {

bool check_if_capture_state_is_useless(ExtendedVAState* state);

class ExtendedVA {

public:
  std::vector<ExtendedVAState*> states;

  ExtendedVA(LogicalVA const &logical_va);

  ExtendedVAState* initial_state() const { return initial_state_; }
  void set_initial_state(ExtendedVAState *s);

  ExtendedVAState* accepting_state() const { return accepting_state_; }
  void set_accepting_state(ExtendedVAState *s);

private:
  ExtendedVAState* initial_state_;
  ExtendedVAState* accepting_state_;

  void capture_closure();

  void copy_data_from_logical_va(LogicalVA &logical_va);
  void copy_transitions_from_logical_va(LogicalVA &logical_va, std::map<unsigned int, ExtendedVAState*> &id_map);
  void copy_states_from_logical_va(LogicalVA &logical_va, std::map<unsigned int, ExtendedVAState*> &id_map);

  std::queue<ExtendedVAState*> inv_topological_sort();
  void inv_topological_sort_util(ExtendedVAState* state, std::queue<ExtendedVAState*> *Q);

  void clean_useless_capture_states();
  void clean_useless_capture_transitions();

  void add_read_captures_transitions();
  void delete_states(std::vector<ExtendedVAState*> &states);
  void remove_filters_and_captures();

  void add_loop_to_initial_state();
};

}

#endif