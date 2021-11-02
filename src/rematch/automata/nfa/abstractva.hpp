#ifndef SRC_REMATCH_AUTOMATA_NFA_ABSTRACTVA_HPP
#define SRC_REMATCH_AUTOMATA_NFA_ABSTRACTVA_HPP

#include <vector>
#include <bitset>

namespace rematch {

class AbsTransition;

class AbsState {
 public:
  virtual std::vector<AbsTransition*> transitions() const = 0;
  virtual std::vector<AbsTransition*> next_transitions(uint code) const = 0;

  virtual bool accepting() const = 0;
  virtual bool initial() const = 0;

}; // class AbsState

class AbsTransition {
 public:
  virtual uint code() const = 0;
  virtual AbsState* prev() const = 0;
  virtual AbsState* next() const = 0;

}; // class AbsTransition

class AbstractVA {
 public:
  virtual const std::vector<AbsState*>& states() const = 0;

  virtual AbsState* accepting_state() const = 0;

  virtual AbsState* initial_state() const = 0;

}; // class AbstractVA

} // namespace rematch

#endif // SRC_REMATCH_AUTOMATA_NFA_ABSTRACTVA_HPP
