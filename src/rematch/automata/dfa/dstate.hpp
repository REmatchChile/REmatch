#ifndef SRC_REMATCH_AUTOMATA_DFA_DSTATE_HPP
#define SRC_REMATCH_AUTOMATA_DFA_DSTATE_HPP

#include <array>
#include <vector>
#include <string>
#include <memory>

#include "structs/ecs/ecs.hpp"

namespace rematch {

class State;

class DState {
 public:

  DState(size_t tot_states);

  void add_state(State* p);

  void add_capture(char a, std::bitset<32> S, DState* s);
  void add_direct(char a, DState* s);
  void add_empty(char a, DState* s);


  std::vector<bool> bitmap() const { return states_bitmap_; }

  long visited;
  internal::ECS::Node* current_node_;

 private:

  uint id_;

  std::array<std::unique_ptr<Transition>, 128> transitions_;

  std::string label_;

  std::vector<bool> states_bitmap_;
  std::vector<State*> states_subset_;

}; // end class DState

} // end namespace rematch


#endif // SRC_REMATCH_AUTOMATA_DFA_DSTATE_HPP
