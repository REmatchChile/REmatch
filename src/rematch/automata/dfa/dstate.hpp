#ifndef AUTOMATA_DFA_DSTATE_HPP
#define AUTOMATA_DFA_DSTATE_HPP

#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "automata/dfa/transition.hpp"
#include "structs/ecs/ecs.hpp"

#include "automata/nfa/lva.hpp"

#include "automata/dfa/dfa.hpp"

namespace rematch {

using StatesBitmap = std::vector<bool>;

class DFA::State {
 public:
  std::string labl;

  internal::ECS::Node *node{nullptr};
  internal::ECS::Node *old_node{nullptr};
  int visited{-1};

  enum Flags {
    kDefaultState = 0,
    kAcceptingState = 1,
    kInitialState = kAcceptingState << 1,
    kLeftAntiAnchor = kInitialState << 1
  };

  State(size_t tot_states);
  State(size_t tot_states, std::vector<LogicalVA::State*> states);
  State(size_t tot_states, std::set<LogicalVA::State*> states);

  void pass_node(internal::ECS::Node *n);

  StatesBitmap bitmap() const { return states_bitmap_; }

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<LogicalVA::State*> Subset of NFA states
  std::vector<LogicalVA::State*> subset() const { return states_subset_; }

  // @brief Returns the subset representation string (i.e. {q1,q2,...}).
  // @return std::string Subset representation of the DState.
  std::string label() const { return labl; }

  int id() const { return id_; }

  bool empty_subset() const { return states_subset_.empty(); }

  bool accepting() const { return flags_ & kAcceptingState; }
  void set_accepting(bool b);

  bool initial() const { return flags_ & kInitialState; }
  void set_initial(bool b);

  void add_direct(char a, State *q);
  void add_capture(char a, std::bitset<32> S, State *q);
  void add_empty(char a);

  Transition<State> *next_transition(char a) const { return transitions_[a]; }

 private:
  static int ID;

  void update_label();
  uint id_;

  std::vector<Transition<State>*> transitions_{128, nullptr};

  StatesBitmap states_bitmap_;
  std::vector<LogicalVA::State*> states_subset_;

  uint8_t flags_ = kDefaultState;

}; // end class DState

} // end namespace rematch

#endif // AUTOMATA_DFA_DSTATE_HPP
