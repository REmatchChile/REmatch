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
  friend class DFA;

  #ifndef NDEBUG
  std::string label_;
  #endif

  State(std::vector<LogicalVA::State*> states);

  ECS::Node* node() const override { return node_;  }
  void set_node(ECS::Node *n) override { node_ = n; }

  int visited() const override { return visited_; }
  void set_visited(int n) override { visited_ = n; }

  // @brief Returns the subset of associated NFA states.
  // @return std::vector<LogicalVA::State*> Subset of NFA states
  std::vector<LogicalVA::State*> subset() const { return states_subset_; }

  // @brief Returns the subset representation string (i.e. {q1,q2,...}).
  // @return std::string Subset representation of the DState.
  // std::string label() const { return labl; }

  int id() const { return id_; }

  bool empty_subset() const { return states_subset_.empty(); }

  bool accepting() const override { return accepting_; }
  void set_accepting(bool b) { accepting_ = b; }

  bool initial() const override { return initial_; }
  void set_initial(bool b) { initial_ = b; }

 private:
  static int ID;

  #ifndef NDEBUG
  void update_label();
  #endif

  unsigned int id_;

  #ifdef NOPT_ASCIIARRAY
  std::unordered_map<std::vector<bool>, Transition> base_transitions_;
  #else
  std::vector<std::optional<Transition>> transitions_{128, std::nullopt};
  #endif

  std::vector<LogicalVA::State*> states_subset_;

  bool accepting_{false};
  bool initial_{false};

  int visited_{-1};

  ECS::Node *node_{nullptr};

}; // end class DState

} // end namespace rematch

#endif // AUTOMATA_DFA_DSTATE_HPP
