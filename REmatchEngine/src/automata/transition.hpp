#ifndef AUTOMATA__TRANSITION_HPP
#define AUTOMATA__TRANSITION_HPP

#include "automata/detstate.hpp"
#include "captures.hpp"

class DetState;
class NodeList;

using DetStates = std::vector<DetState*>;

namespace rematch {

class Transition {
 public:
  virtual void visit(int64_t i, NodeList *prev_list, DetStates &new_states) = 0;
  virtual std::vector<Capture*> captures() const = 0;
  virtual Transition* add_capture(Capture* capture) = 0;
  virtual Transition* add_direct(DetState* state) = 0;
}; // class Transition

class NoCapture : public Transition {
 public:
  friend class NoOneCapture;

  NoCapture(DetState* next_state);

  virtual void visit(int64_t i, NodeList *prev_list, DetStates &new_states);
  virtual std::vector<Capture*> captures() const;
  virtual Transition* add_capture(Capture* capture);
  virtual Transition* add_direct(DetState* state);

  DetState* next() {return next_;}
 private:
  DetState* next_;
}; // class NoCapture

class OneCapture : public Transition {
 public:
  friend class NoOneCapture;
  friend class MultiCapture;

  OneCapture(Capture* capture);

  virtual void visit(int64_t i, NodeList *prev_list, DetStates &new_states);
  virtual std::vector<Capture*> captures() const;
  virtual Transition* add_capture(Capture* capture);
  virtual Transition* add_direct(DetState* state);
 private:
  Capture *capture_;
}; // class OneCapture

class MultiCapture : public Transition {
 public:
  friend class NoMultiCapture;

  MultiCapture(OneCapture &transition, Capture* second_capture);

  virtual void visit(int64_t i, NodeList *prev_list, DetStates &new_states);
  virtual std::vector<Capture*> captures() const;
  virtual Transition* add_capture(Capture* capture);
  virtual Transition* add_direct(DetState* state);
 private:
  std::vector<Capture*> captures_;
}; // class MultiCapture

class NoOneCapture : public Transition {
 public:
  friend class NoMultiCapture;

  NoOneCapture(OneCapture &transition, DetState* next_state);
  NoOneCapture(NoCapture &transition, Capture* second_capture);

  virtual void visit(int64_t i, NodeList *prev_list, DetStates &new_states);
  virtual std::vector<Capture*> captures() const;
  virtual Transition* add_capture(Capture* capture);
  virtual Transition* add_direct(DetState* state);
 private:
  Capture *capture_;
  DetState *next_;
}; // class NoOneCapture

class NoMultiCapture : public Transition {
 public:

  NoMultiCapture(NoOneCapture& transition, Capture* second_capture);
  NoMultiCapture(MultiCapture& transition, DetState* next_state);

  virtual void visit(int64_t i, NodeList *prev_list, DetStates &new_states);
  virtual std::vector<Capture*> captures() const;
  virtual Transition* add_capture(Capture* capture);
  virtual Transition* add_direct(DetState* state);
 private:
  std::vector<Capture*> captures_;
  DetState *next_;
}; // class NoMultiCapture

} // namespace rematch

#endif // AUTOMATA__TRANSITION_HPP