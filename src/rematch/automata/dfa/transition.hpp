#ifndef AUTOMATA_DFA_TRANSITION_HPP
#define AUTOMATA_DFA_TRANSITION_HPP

#include <bitset>
#include <memory>
#include <stdexcept>

namespace rematch {

namespace abstract {
class DState;
}

struct Transition {

  struct Capture {
    std::bitset<32> S;
    abstract::DState *next{nullptr};
  };

  enum Type {
    kEmpty                    = 0,        // 0000
    kDirect                   = 1 << 0,   // 0001
    kSingleCapture            = 1 << 1,   // 0010
    kMultiCapture             = 1 << 2,   // 0100
    kMultiDirect              = 1 << 3,   // 1000
    kDirectSingleCapture      = kDirect       | kSingleCapture,
    kDirectMultiCapture       = kDirect       | kMultiCapture,
    kMultiDirectSingleCapture = kMultiDirect  | kSingleCapture,
    kMultiDirectMultiCapture  = kMultiDirect  | kMultiCapture
  };

  int type_;
  abstract::DState *direct_{nullptr};
  Capture capture_;
  std::vector<Capture> captures_;
  std::vector<abstract::DState*> directs_;

  // Default = EmptyTransition
  Transition() : type_(Type::kEmpty) {}

  Transition(std::bitset<32> S, abstract::DState *state)
      : type_(Type::kSingleCapture), capture_({S, state}) {}

  Transition(abstract::DState *state) : type_(Type::kDirect), direct_(state) {}

  void add(Capture capture) {
    if (capture.S != 0)
      add_capture(capture);
    else
      add_direct(capture.next);
  }

  void add_capture(Capture capture) {
    switch (type_) {
    case Type::kEmpty:
      type_ = kSingleCapture;
      capture_ = capture;
      break;
    case Type::kDirect:
      type_ = Type::kDirectSingleCapture;
      capture_ = capture;
      break;
    case Type::kMultiDirect:
      type_ = Type::kMultiDirectSingleCapture;
      capture_ = capture;
      break;
    case Type::kSingleCapture:
      //* Do not push_back capture_ into captures_, necessary for refcounts.
      type_ = Type::kMultiCapture;
      captures_.clear();
      captures_.push_back(capture);
      break;
    case Type::kDirectSingleCapture:
      type_ = Type::kDirectMultiCapture;
    case Type::kMultiDirectSingleCapture:
      //* Do push_back capture_ into captures_, necessary for refcounts.
      type_ = Type::kMultiDirectMultiCapture;
      captures_.clear();
      captures_.push_back(capture_);
      captures_.push_back(capture);
      break;
    default:
      captures_.push_back(capture);
      break;
    }
  }

  void add_direct(abstract::DState *state) {
    switch (type_) {
    case Type::kEmpty:
      type_ = Type::kDirect;
      direct_ = state;
      break;
    case Type::kDirect:
      type_ = kMultiDirect;
      directs_.clear();
      directs_.push_back(state);
      break;
    case Type::kDirectSingleCapture:
      type_ = kMultiDirectSingleCapture;
      directs_.clear();
      directs_.push_back(state);
      break;
    case Type::kDirectMultiCapture:
      type_ = kMultiDirectMultiCapture;
      directs_.clear();
      directs_.push_back(state);
      break;
    case Type::kSingleCapture:
      type_ = Type::kDirectSingleCapture;
      direct_ = state;
      break;
    case Type::kMultiCapture:
      type_ = Type::kDirectMultiCapture;
      captures_.push_back(capture_);
      direct_ = state;
      break;
    default: // kMultiDirect, kMultiDirectSingleCapture, kMultiDirectMultiCapture
      directs_.push_back(state);
      break;
    }
  }
}; // end struct Transition

} // namespace rematch

#endif // AUTOMATA_DFA_TRANSITION_HPP
