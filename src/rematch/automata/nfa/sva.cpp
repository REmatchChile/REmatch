#include "sva.hpp"

#include <iostream>

namespace rematch {

SearchVA::SearchVA(LogicalVA const &A) : has_epsilon_(A.has_epsilon()) {
  LogicalVA A_prim(A);

  A_prim.trim();

  A_prim.remove_captures();

  std::cout << "SearchVA:\n" << A_prim.pprint() << '\n';

  states_.swap(A_prim.states);
  initial_state_ = A_prim.initial_state();
  accepting_state_ = A_prim.accepting_state();
}

// ---  Getters  ---  //

std::vector<State*> const & SearchVA::states() const { return states_; }

State* SearchVA::initial_state() const { return initial_state_; }

State* SearchVA::accepting_state() const { return accepting_state_; }

bool SearchVA::has_epsilon() const { return has_epsilon_; }



} // end namespace rematch