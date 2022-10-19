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
  class State : public abstract::DState {
   public:
    friend class UnambiguousVA;

    State(LogicalVA::State const &s)
        : id(s.id), initial_(s.initial()), accepting_(s.accepting()) {}

    State() = delete;

    // TODO: Destructor virtual

    ECS::Node* node() const override { return node_; }
    void set_node(ECS::Node *n) override { node_ = n; }

    int visited() const override { return visited_; }
    void set_visited(int n) override { visited_ = n; }

    bool accepting() const override { return accepting_; }
    bool initial() const override { return initial_; }

   private:

    int id;
    std::vector<std::optional<Transition>> transitions_{128, std::nullopt};
    bool initial_;
    bool accepting_;

    int visited_{-1};
    ECS::Node *node_{nullptr};

  }; // end struct UnambiguousVA::State

  using UVACaptureStatesTable = std::vector<std::pair<State*, std::bitset<32>>>;

  UnambiguousVA(ExtendedVA const &A);

  UnambiguousVA() = delete;

  State* init_state() { return initial_state_; }

  UVACaptureStatesTable init_eval_states() { return init_eval_states_; }

  Transition next_transition(abstract::DState *q, char a);

  size_t size() const { return states_.size(); }

 private:

  // Search corresponding ustate, if not found then it's allocated.
  State* obtain_ustate(LogicalVA::State* p);

  State* initial_state_;
  State* accepting_state_;

  std::vector<State> states_;
  std::vector<Transition> transitions_;

  std::shared_ptr<FilterFactory> ffactory_;

  boost::bimap<State*, LogicalVA::State*> state_bimap_;

  UVACaptureStatesTable init_eval_states_;
}; // end class UnambiguousVA

} // end namespace rematch

#endif // AUTOMATA_NFA_UVA_HPP
