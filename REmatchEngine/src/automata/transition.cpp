#include "transition.hpp"

#include <sstream>
#include <stdexcept>

#include "eval.hpp"

namespace rematch {

void EmptyTransition::visit(int64_t i, NodeList *prev_list, DetStates &new_states) {
  prev_list->resetRefs();
  Evaluator::memory_manager_.addPossibleGarbage(prev_list->head);
}

std::vector<Capture*> EmptyTransition::captures() const {
  return std::vector<Capture*>();
}

Transition* EmptyTransition::add_capture(Capture* capture) {
  throw std::logic_error("Can't add capture to empty transition.");
}

Transition* EmptyTransition::add_direct(DetState* state) {
  throw std::logic_error("Can't add direct to empty transition.");
}

NoCapture::NoCapture(DetState* next_state): next_(next_state) {type_ = 1;}

void NoCapture::visit(int64_t i, NodeList *prev_list, DetStates &new_states) {
  if(next_->visited <= i+1) {
    next_->visited = i+2;

    // Lazy copy
    next_->currentL->head = prev_list->head;
    next_->currentL->tail = prev_list->tail;

    new_states.push_back(next_);
  } else {
    next_->currentL->append(prev_list);
  }
}

std::vector<Capture*> NoCapture::captures() const {
  return std::vector<Capture*>();
}

Transition* NoCapture::add_capture(Capture* capture) {
  return new NoOneCapture(*this, capture);
}

Transition* NoCapture::add_direct(DetState* state) {
  throw std::logic_error("Can't add direct to already present direct.");
}

OneCapture::OneCapture(Capture* capture): capture_(capture) {type_ = 2;}

void OneCapture::visit(int64_t i, NodeList *prev_list, DetStates &new_states) {
  Node* new_node = Evaluator::memory_manager_.alloc(capture_->S,
                                                  i+1,
                                                  prev_list->head,
                                                  prev_list->tail);
  if(capture_->next->visited <= i+1) {
      capture_->next->visited = i+2;
      capture_->next->currentL->resetAndAdd(new_node);
      new_states.push_back(capture_->next);
    } else {
      capture_->next->currentL->add(new_node);
    }
}

Transition* OneCapture::add_capture(Capture* capture) {
  return new MultiCapture(*this, capture);
}

Transition* OneCapture::add_direct(DetState* state) {
  return new NoOneCapture(*this, state);
}

MultiCapture::MultiCapture(OneCapture &transition, Capture* second_capture) {
  type_ = 3;
  captures_.push_back(transition.capture_);
  captures_.push_back(second_capture);
}

void MultiCapture::visit(int64_t i, NodeList *prev_list, DetStates &new_states) {
  for(auto &capture: captures_) {
    Node* new_node = Evaluator::memory_manager_.alloc(capture->S,
                                                      i+1,
                                                      prev_list->head,
                                                      prev_list->tail);
    if(capture->next->visited <= i+1) {
      capture->next->visited = i+2;
      capture->next->currentL->resetAndAdd(new_node);
      new_states.push_back(capture->next);
    } else {
      capture->next->currentL->add(new_node);
    }
  }
}

Transition* MultiCapture::add_capture(Capture* capture) {
  captures_.push_back(capture);
  return this;
}

Transition* MultiCapture::add_direct(DetState* state) {
  return new NoMultiCapture(*this, state);
}

NoOneCapture::NoOneCapture(OneCapture &transition, DetState* next_state)
  : capture_(transition.capture_), next_(next_state) {type_ = 4;}

NoOneCapture::NoOneCapture(NoCapture &transition, Capture* capture)
  : capture_(capture), next_(transition.next_) {type_ = 4;}

void NoOneCapture::visit(int64_t i, NodeList *prev_list, DetStates &new_states) {
  if(next_->visited <= i+1) {
    next_->visited = i+2;

    // Lazy copy
    next_->currentL->head = prev_list->head;
    next_->currentL->tail = prev_list->tail;

    new_states.push_back(next_);
  } else {
    next_->currentL->append(prev_list);
  }

  Node* new_node = Evaluator::memory_manager_.alloc(capture_->S,
                                                    i+1,
                                                    prev_list->head,
                                                    prev_list->tail);
  if(capture_->next->visited <= i+1) {
    capture_->next->visited = i+2;
    capture_->next->currentL->resetAndAdd(new_node);
    new_states.push_back(capture_->next);
  } else {
    capture_->next->currentL->add(new_node);
  }
}

Transition* NoOneCapture::add_capture(Capture* capture) {
  return new NoMultiCapture(*this, capture);
}

Transition* NoOneCapture::add_direct(DetState* state) {
  throw std::logic_error("Can't add direct to already present direct.");
}

NoMultiCapture::NoMultiCapture(NoOneCapture& transition, Capture* second_capture)
  : next_(transition.next_) {
  type_ = 5;
  captures_.push_back(transition.capture_);
  captures_.push_back(second_capture);
}

NoMultiCapture::NoMultiCapture(MultiCapture& transition, DetState* next_state)
  : captures_(transition.captures_), next_(next_state) {type_ = 5;}

void NoMultiCapture::visit(int64_t i, NodeList *prev_list, DetStates &new_states) {
  if(next_->visited <= i+1) {
    next_->visited = i+2;

    // Lazy copy
    next_->currentL->head = prev_list->head;
    next_->currentL->tail = prev_list->tail;

    new_states.push_back(next_);
  } else {
    next_->currentL->append(prev_list);
  }
  for(auto &capture: captures_) {
    Node* new_node = Evaluator::memory_manager_.alloc(capture->S,
                                                      i+1,
                                                      prev_list->head,
                                                      prev_list->tail);
    if(capture->next->visited <= i+1) {
      capture->next->visited = i+2;
      capture->next->currentL->resetAndAdd(new_node);
      new_states.push_back(capture->next);
    } else {
      capture->next->currentL->add(new_node);
    }
  }
}

Transition* NoMultiCapture::add_capture(Capture* capture) {
  captures_.push_back(capture);
  return this;
}

Transition* NoMultiCapture::add_direct(DetState* state) {
  throw std::logic_error("Can't add direct to already present direct.");
}


std::vector<Capture*> OneCapture::captures() const {
  std::vector<Capture*> v;
  v.push_back(capture_);
  return v;
}

std::vector<Capture*> NoOneCapture::captures() const {
  std::vector<Capture*> v;
  v.push_back(capture_);
  return v;
}

std::vector<Capture*> NoMultiCapture::captures() const {return captures_;}
std::vector<Capture*> MultiCapture::captures() const {return captures_;}

} // namespace rematch