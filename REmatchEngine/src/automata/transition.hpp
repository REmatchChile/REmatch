#ifndef AUTOMATA__TRANSITION_HPP
#define AUTOMATA__TRANSITION_HPP

#include <memory>

#include "automata/detstate.hpp"
#include "captures.hpp"

class DetState;
class NodeList;

using DetStates = std::vector<DetState*>;
using Captures = std::vector<std::unique_ptr<Capture>>;

namespace rematch {

struct Transition {
  enum Type {
    kEmpty                = 0,
    kDirect               = 1<<1,
    kSingleCapture        = 1<<2,
    kDirectSingleCapture  = kDirect & kSingleCapture,
    kMultiCapture         = 1<<3,
    kDirectMultiCapture   = kDirect & kMultiCapture
  };

  int type_;
  DetState* direct_;
  std::unique_ptr<Capture> capture_;
  Captures captures_;

  // Default = EmptyTransition
  Transition(): type_(0) {}

  Transition(std::unique_ptr<Capture> capture)
    : type_(2),
      capture_(std::move(capture)) {}

  Transition(DetState* state): type_(1), direct_(state) {}

  void add_capture(std::unique_ptr<Capture> capture) {
    switch (type_) {
      case 0:
        throw std::logic_error("Can't add capture to empty transition.");
        break;
      case 1:
        type_ = 3;
        capture_ = std::move(capture);
        break;
      case 2:
        type_ = 4;
        captures_.push_back(std::move(capture_));
        captures_.push_back(std::move(capture));
        break;
      case 3:
        type_ = 5;
        captures_.push_back(std::move(capture_));
        captures_.push_back(std::move(capture));
        break;
      default:
        captures_.push_back(std::move(capture));
        break;
    }
  }

  void add_direct(DetState* state) {
    switch (type_) {
      case 0:
        throw std::logic_error("Can't add direct to empty transition.");
        break;
      case 1:
      case 3:
      case 5:
        throw std::logic_error("Can't add direct because a direct is already present.");
        break;
      case 2:
        type_ = 3;
        direct_ = state;
        break;
      case 4:
        type_ = 5;
        direct_ = state;
        break;
    }
  }
}; // end struct Transition

} // namespace rematch

#endif // AUTOMATA__TRANSITION_HPP