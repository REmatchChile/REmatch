#ifndef AUTOMATA_DFA_DSTATE_HPP
#define AUTOMATA_DFA_DSTATE_HPP

#include <array>
#include <vector>
#include <string>
#include <memory>
#include <set>

#include "structs/ecs/ecs.hpp"
#include "automata/dfa/transition.hpp"

namespace rematch {

class State;
class DState;

using StatesBitmap = std::vector<bool>;

class DState {
 public:

  internal::ECS::Node* node;
  int visited = -1;

  enum Flags {
    kDefaultState    =  0,
    kAcceptingState  =  1,
    kInitialState    =  kAcceptingState << 1,
    kLeftAntiAnchor  =  kInitialState  << 1
  };

  DState(size_t tot_states);
  DState(size_t tot_states, std::vector<State*> states);
  DState(size_t tot_states, std::set<State*> states);

  void add_state(State* p);

  void pass_node(internal::ECS::Node* n);

  StatesBitmap bitmap() const { return states_bitmap_; }

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<State*> Subset of NFA states
  std::vector<State*> subset() const { return states_subset_; }

  // @brief Returns the subset representation string (i.e. {q1,q2,...}).
  // @return std::string Subset representation of the DState.
  std::string label() const { return label_; }

  int id() const { return id_; }

  bool empty_subset() const { return states_subset_.empty(); }

  bool accepting() const { return  flags_ & kAcceptingState; }
  void set_accepting(bool b);

  bool initial() const { return flags_ & kInitialState; }
  void set_initial(bool b);

  void add_direct(char a, DState* q);
  void add_capture(char a, std::bitset<32> S, DState* q);
  void add_empty(char a);

  Transition* next_transition(char a) const {
    return transitions_[a];
  }

 private:
  static int ID;

  uint id_;

  std::vector<Transition*> transitions_{128, nullptr};

  std::string label_;

  StatesBitmap states_bitmap_;
  std::vector<State*> states_subset_;

  uint8_t flags_ = kDefaultState;

}; // end class DState

} // end namespace rematch


#endif // AUTOMATA_DFA_DSTATE_HPP
