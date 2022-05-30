#ifndef AUTOMATA_DFA_TRANSITION_HPP
#define AUTOMATA_DFA_TRANSITION_HPP

#include <bitset>
#include <memory>
#include <stdexcept>

namespace rematch {

class NodeList;
class DState;

struct DCapture {
  std::bitset<32> S;
  DState *next;

  DCapture(std::bitset<32> S, DState *q) : S(S), next(q) {}
};

using DStates = std::vector<DState *>;
using Captures = std::vector<std::unique_ptr<DCapture>>;

struct Transition {
  enum Type {
    kEmpty = 0,
    kDirect = 1 << 0,
    kSingleCapture = 1 << 1,
    kDirectSingleCapture = kDirect | kSingleCapture,
    kMultiCapture = 1 << 2,
    kDirectMultiCapture = kDirect | kMultiCapture
  };

  int type_;
  DState *direct_;
  DCapture *capture_;
  std::vector<DCapture *> captures_;

  // Default = EmptyTransition
  Transition() : type_(Type::kEmpty) {}

  Transition(DCapture *capture)
      : type_(Type::kSingleCapture), capture_(capture) {}

  Transition(DState *state) : type_(Type::kDirect), direct_(state) {}

  void add_capture(DCapture *capture) {
    switch (type_) {
    case Type::kEmpty:
      throw std::logic_error("Can't add capture to empty transition.");
      break;
    case Type::kDirect:
      type_ = Type::kDirectSingleCapture;
      capture_ = capture;
      break;
    case Type::kSingleCapture:
      type_ = Type::kMultiCapture;
      captures_.clear();
      captures_.push_back(capture_);
      captures_.push_back(capture);
      break;
    case Type::kDirectSingleCapture:
      type_ = Type::kDirectMultiCapture;
      captures_.clear();
      captures_.push_back(capture_);
      captures_.push_back(capture);
      break;
    default:
      captures_.push_back(capture);
      break;
    }
  }

  void add_direct(DState *state) {
    switch (type_) {
    case Type::kEmpty:
      throw std::logic_error("Can't add direct to empty transition.");
      break;
    case Type::kDirect:
    case Type::kDirectSingleCapture:
    case Type::kDirectMultiCapture:
      throw std::logic_error(
          "Can't add direct because a direct is already present.");
      break;
    case Type::kSingleCapture:
      type_ = Type::kDirectSingleCapture;
      direct_ = state;
      break;
    case Type::kMultiCapture:
      type_ = Type::kDirectMultiCapture;
      direct_ = state;
      break;
    }
  }
}; // end struct Transition

} // namespace rematch

#endif // AUTOMATA_DFA_TRANSITION_HPP
