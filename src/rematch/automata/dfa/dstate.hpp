#ifndef AUTOMATA_DFA_DSTATE_HPP
#define AUTOMATA_DFA_DSTATE_HPP

#include <array>
#include <vector>
#include <string>
#include <memory>

#include "structs/ecs/ecs.hpp"
#include "automata/dfa/transition.hpp"

namespace rematch {

class State;
class DState;

class DState {
 public:

  enum StateFlags {
    kDefaultState    =  0,
    kAcceptingState  =  1,
    kInitialState    =  kAcceptingState << 1,
    kLeftAntiAnchor  =  kInitialState  << 1
  };

  DState(size_t tot_states);
  DState(size_t tot_states, std::vector<State*> states);

  void add_state(State* p);

  void add_transition(char a, DState* s);

  std::vector<bool> bitmap() const { return states_bitmap_; }

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<State*> Subset of NFA states
  std::vector<State*> subset() const { return states_subset_; }

  bool empty_subset() const { return states_subset_.empty(); }

  bool accepting() const { return  flags_ & kAcceptingState; }
  bool initial() const { return flags_ & kInitialState; }

  DState* next_state(char a) const { return transitions_[a]; }

 private:

  uint id_;

  std::array<DState*, 128> transitions_;

  std::string label_;

  std::vector<bool> states_bitmap_;
  std::vector<State*> states_subset_;

  uint8_t flags_ = kDefaultState;

}; // end class DState

} // end namespace rematch


#endif // AUTOMATA_DFA_DSTATE_HPP
