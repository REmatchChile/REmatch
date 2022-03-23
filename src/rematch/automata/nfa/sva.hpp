#ifndef SRC_REMATCH_AUTOMATA_NFA_SVA_HPP
#define SRC_REMATCH_AUTOMATA_NFA_SVA_HPP

#include <vector>

#include "automata/nfa/state.hpp"
#include "automata/nfa/lva.hpp"
#include "regex/regex_options.hpp"

namespace rematch {

// A non-deterministic automaton optimized for output fast searching.
class SearchVA {
 public:
  SearchVA() = delete;
  SearchVA(LogicalVA const &A, Anchor a);

  std::vector<State*> const & states() const;

  State* initial_state() const;
  State* accepting_state() const;

  bool has_epsilon() const;

  size_t size() const { return states_.size(); }

  std::shared_ptr<FilterFactory> filter_factory() const { return ffactory_; }
  std::shared_ptr<VariableFactory> variable_factory() const { return vfactory_; }

 private:

  std::vector<State*> states_;
  State* initial_state_;
  State* accepting_state_;

  bool has_epsilon_;

  std::shared_ptr<FilterFactory> ffactory_;
  std::shared_ptr<VariableFactory> vfactory_;

  Anchor anchor_;
}; // end class SearchVA

} // end namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_SVA_HPP
