#ifndef AUTOMATA_DFA_SDSTATE_HPP
#define AUTOMATA_DFA_SDSTATE_HPP

#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "automata/dfa/transition.hpp"
#include "structs/ecs/ecs.hpp"

#include "automata/nfa/lva.hpp"

namespace rematch {

class SDState;

class SDState {
public:
  SDState(size_t tot_states);
  SDState(size_t tot_states, std::vector<LogicalVA::State *> states);
  SDState(size_t tot_states, std::set<LogicalVA::State *> states);

  void add_state(LogicalVA::State *p);

  std::vector<bool> bitmap() const { return states_bitmap_; }

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<LogicalVA::State*> Subset of NFA states
  std::vector<LogicalVA::State *> subset() const { return states_subset_; }

  int id() const { return id_; }

  bool accepting() const { return accepting_; }
  void set_accepting(bool b) { accepting_ = b; }

  bool initial() const { return initial_; }
  void set_initial(bool b) { initial_ = b; }

  bool ends() const { return ends_; }
  void set_ends(bool b) { ends_ = b; }

  void set_transition(char a, SDState *q) { transitions_[a] = q; }

  SDState *next_state(char a) const { return transitions_[a]; }

private:
  static int ID;

  unsigned int id_;

  std::vector<SDState *> transitions_{128, nullptr};

  std::vector<bool> states_bitmap_;
  std::vector<LogicalVA::State *> states_subset_;

  bool initial_{false};
  bool accepting_{false};
  bool ends_{false};

}; // end class DState

} // end namespace rematch

#endif // AUTOMATA_DFA_SDSTATE_HPP
