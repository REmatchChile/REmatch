#ifndef AUTOMATA_DFA_DSTATE_HPP
#define AUTOMATA_DFA_DSTATE_HPP

#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "automata/dfa/transition.hpp"
#include "automata/abs_dstate.hpp"
#include "structs/ecs/ecs.hpp"

#include "automata/nfa/lva.hpp"

#include "automata/dfa/dfa.hpp"

namespace rematch {

using StatesBitmap = std::vector<bool>;

class DFA::State : public abstract::DState {

 public:
  std::string labl;

  ECS::Node *old_node{nullptr};

  State(size_t tot_states);
  State(size_t tot_states, std::vector<LogicalVA::State*> states);
  State(size_t tot_states, std::set<LogicalVA::State*> states);

  ECS::Node* node() const override { return node_;  }
  void set_node(ECS::Node *n) override { node_ = n; }

  int visited() const override { return visited_; }
  void set_visited(int n) override { visited_ = n; }

  StatesBitmap bitmap() const { return states_bitmap_; }

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<LogicalVA::State*> Subset of NFA states
  std::vector<LogicalVA::State*> subset() const { return states_subset_; }

  // @brief Returns the subset representation string (i.e. {q1,q2,...}).
  // @return std::string Subset representation of the DState.
  std::string label() const { return labl; }

  int id() const { return id_; }

  bool empty_subset() const { return states_subset_.empty(); }

  bool accepting() const override { return accepting_; }
  void set_accepting(bool b) { accepting_ = b; }

  bool initial() const override { return initial_; }
  void set_initial(bool b) { initial_ = b; }

  void add_direct(char a, State *q);
  void add_capture(char a, std::bitset<32> S, State *q);
  void add_empty(char a);

  std::optional<Transition> next_transition(char a) const override {
    return transitions_[a];
  }

 private:
  static int ID;

  void update_label();

  uint id_;

  std::vector<std::optional<Transition>> transitions_{128, std::nullopt};

  StatesBitmap states_bitmap_;
  std::vector<LogicalVA::State*> states_subset_;

  bool accepting_;
  bool initial_;

  int visited_{-1};

  ECS::Node *node_{nullptr};

}; // end class DState

} // end namespace rematch

#endif // AUTOMATA_DFA_DSTATE_HPP
