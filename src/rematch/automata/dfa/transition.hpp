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
  std::unique_ptr<Capture> capture_;
  std::unique_ptr<Captures> captures_;

  // Default = EmptyTransition
  Transition(): type_(Type::kEmpty) {}

  Transition(std::unique_ptr<Capture> capture)
    : type_(Type::kSingleCapture),
      capture_(std::move(capture)) {}

  Transition(DetState* state): type_(Type::kDirect), direct_(state) {}

  void add_capture(std::unique_ptr<Capture> capture) {
    switch (type_) {
      case Type::kEmpty:
        throw std::logic_error("Can't add capture to empty transition.");
        break;
      case Type::kDirect:
        type_ = Type::kDirectSingleCapture;
        capture_ = std::move(capture);
        break;
      case Type::kSingleCapture:
        type_ = Type::kMultiCapture;
        captures_ = std::make_unique<Captures>();
        captures_->push_back(std::move(capture_));
        captures_->push_back(std::move(capture));
        break;
      case Type::kDirectSingleCapture:
        type_ = Type::kDirectMultiCapture;
        captures_ = std::make_unique<Captures>();
        captures_->push_back(std::move(capture_));
        captures_->push_back(std::move(capture));
        break;
      default:
        captures_->push_back(std::move(capture));
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