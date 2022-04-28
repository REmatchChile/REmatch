#ifndef AUTOMATA_DFA_SDSTATE_HPP
#define AUTOMATA_DFA_SDSTATE_HPP

#include <array>
#include <vector>
#include <string>
#include <memory>
#include <set>

#include "structs/ecs/ecs.hpp"
#include "automata/dfa/transition.hpp"

namespace rematch {

class State;
class SDState;

class SDState {
 public:

  SDState(size_t tot_states);
  SDState(size_t tot_states, std::vector<State*> states);
  SDState(size_t tot_states, std::set<State*> states);

  void add_state(State* p);

  std::vector<bool> bitmap() const { return states_bitmap_; }

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<State*> Subset of NFA states
  std::vector<State*> subset() const { return states_subset_; }

  int id() const { return id_; }

  bool accepting() const { return accepting_; }
  void set_accepting(bool b) { accepting_ = b; }

  bool initial() const { return initial_; }
  void set_initial(bool b) { initial_ = b; }

  void set_transition(char a, SDState* q) { transitions_[a] = q; }

  SDState* next_state(char a) const { return transitions_[a]; }

 private:
  static int ID;

  uint id_;

  std::vector<SDState*> transitions_{128, nullptr};

  std::vector<bool> states_bitmap_;
  std::vector<State*> states_subset_;

  bool initial_{false};
  bool accepting_{false};

}; // end class DState

} // end namespace rematch


#endif // AUTOMATA_DFA_SDSTATE_HPP
