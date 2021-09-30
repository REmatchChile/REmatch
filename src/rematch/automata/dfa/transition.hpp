#ifndef AUTOMATA__TRANSITION_HPP
#define AUTOMATA__TRANSITION_HPP

#include <memory>

#include "automata/dfa/detstate.hpp"
#include "captures.hpp"

namespace rematch {

class DetState;
class NodeList;

using DetStates = std::vector<DetState*>;
using Captures = std::vector<std::unique_ptr<Capture>>;

struct Transition {
  enum Type {
    kEmpty                = 0,
    kDirect               = 1<<0,
    kSingleCapture        = 1<<1,
    kDirectSingleCapture  = kDirect | kSingleCapture,
    kMultiCapture         = 1<<2,
    kDirectMultiCapture   = kDirect | kMultiCapture
  };

  int type_;
  DetState* direct_;
  Capture* capture_;
  std::vector<Capture*> captures_;

  // Default = EmptyTransition
  Transition(): type_(Type::kEmpty) {}

  Transition(Capture* capture)
    : type_(Type::kSingleCapture),
      capture_(capture) {}

  Transition(DetState* state): type_(Type::kDirect), direct_(state) {}

  void add_capture(Capture* capture) {
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

  void add_direct(DetState* state) {
    switch (type_) {
      case Type::kEmpty:
        throw std::logic_error("Can't add direct to empty transition.");
        break;
      case Type::kDirect:
      case Type::kDirectSingleCapture:
      case Type::kDirectMultiCapture:
        throw std::logic_error("Can't add direct because a direct is already present.");
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

#endif // AUTOMATA__TRANSITION_HPP