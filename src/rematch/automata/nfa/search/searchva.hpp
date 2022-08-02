#ifndef SRC_REMATCH_AUTOMATA_NFA_SEARCH_SEARCHVA_HPP
#define SRC_REMATCH_AUTOMATA_NFA_SEARCH_SEARCHVA_HPP

#include <vector>

#include "automata/nfa/abstractva.hpp"
#include "automata/nfa/lva.hpp"

class SearchVA : public AbstractVA {
 public:
  class State;
  class Transition;

  SearchVA(const &LogicalVA A);

  virtual State *initial_state() const { return initial_state_; }
  virtual State *accepting_state() const { return accepting_state_; }

  virtual const std::vector<State *> &states() const { return states_; }

private:
  std::vector<State *> states_;
  std::vector<Transition> transitions_;

  State *initial_state_;
  State *accepting_state_;

  bool epsilon_; // If epsilon is in the language

}; // enc class SearchVa

#endif // SRC_REMATCH_AUTOMATA_NFA_SEARCH_SEARCHVA_HPP
