#ifndef AUTOMATA_NFA_UVA_HPP
#define AUTOMATA_NFA_UVA_HPP

#include "automata/nfa/eva.hpp"
#include "automata/dfa/dstate.hpp"
#include "structs/ecs/ecs.hpp"
#include "automata/nfa/state.hpp"

#include <boost/bimap.hpp>

namespace rematch {

class UnambiguousVA {
 public:
  struct State {

    State(LogicalVA::State const &s)
        : id(s.id), initial(s.initial()), accepting_(s.accepting()) {}

    State() = delete;

    Transition<State> *next_transition(char a) const { return transitions[a];}

    void pass_node(internal::ECS::Node *n) { node = n; }

    bool accepting() const { return accepting_; }

    int id;
    std::vector<Transition<State>*> transitions{128, nullptr};
    bool initial;
    bool accepting_;

    int visited{-1};
    internal::ECS::Node *node{nullptr};

  }; // end struct UnambiguousVA::State

  using UVACaptureStatesTable = std::vector<std::pair<State*, std::bitset<32>>>;

  UnambiguousVA(ExtendedVA const &A);

  UnambiguousVA() = delete;

  State* init_state() { return initial_state_; }

  UVACaptureStatesTable init_eval_states() { return init_eval_states_; }

  Transition<State>* next_transition(State *q, char a);

  size_t size() const { return states_.size(); }

 private:

  // Search corresponding ustate, if not found then it's allocated.
  State* obtain_ustate(LogicalVA::State* p);

  State* initial_state_;
  State* accepting_state_;

  std::vector<State> states_;
  std::vector<Transition<State>> transitions_;

  std::shared_ptr<FilterFactory> ffactory_;

  boost::bimap<State*, LogicalVA::State*> state_bimap_;

  UVACaptureStatesTable init_eval_states_;
}; // end class UnambiguousVA

} // end namespace rematch

#endif // AUTOMATA_NFA_UVA_HPP
