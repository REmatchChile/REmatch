#ifndef EXTENDED_VA_HPP
#define EXTENDED_VA_HPP

#include "../parsing/logical_variable_set_automaton/logical_va.hpp"
#include "extended_va_state.hpp"
#include <queue>
#include <map>

namespace rematch {

class ExtendedVA {

public:
  std::vector<ExtendedVAState*> states;

  ExtendedVA(LogicalVA const &A);

  ExtendedVAState* initial_state() const { return initial_state_; }
  void set_initial_state(ExtendedVAState *s) { initial_state_ = s; }

  ExtendedVAState* accepting_state() const { return accepting_state_; }
  void set_accepting_state(ExtendedVAState *s);

  bool is_static() const { return is_static_; }
  void set_is_static(bool b) { is_static_ = b; }

private:
  ExtendedVAState* initial_state_;
  ExtendedVAState* accepting_state_;

  void capture_closure();

  // void relabel_states();

  void copy_data_from_logical_va(LogicalVA A);
  void copy_transitions_from_logical_va(LogicalVA A, std::map<unsigned int, ExtendedVAState*> &id_map);
  void copy_states_from_logical_va(LogicalVA A, std::map<unsigned int, ExtendedVAState*> &id_map);

  std::queue<ExtendedVAState*> inv_topological_sort();
  void ExtendedVA::inv_topological_sort_util(ExtendedVAState* state, std::queue<ExtendedVAState*> *Q);

  void clean_useless_capture_states();
  void clean_useless_capture_transitions();

  bool ExtendedVA::check_if_static();
  bool is_static_;
	std::bitset<64> static_S_{0};
};

}

#endif