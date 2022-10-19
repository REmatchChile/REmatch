#include "state.hpp"

#include <bitset>
#include <list>
#include <set>

namespace rematch {

LogicalVA::Capture::Capture(State *from, std::bitset<32> coding, State *next)
    : from(from), next(next), code(coding) {}

bool LogicalVA::Capture::operator==(const Capture &rhs) const {
  return (code == rhs.code && next == rhs.next && from == rhs.from);
}

bool LogicalVA::Capture::operator<(const LogicalVA::Capture &rhs) const {
  /* Simple ordering for being able to construct an std::set<LVACapture> */
  if (code.to_ulong() != rhs.code.to_ulong()) {
    return code.to_ulong() < rhs.code.to_ulong();
  }

  else if (next != rhs.next) {
    return next < rhs.next;
  }

  return from < rhs.from;
}

LogicalVA::State::State() : flags_(kDefaultState) { id = ID++; }

LogicalVA::State::State(const State &s) : flags_(s.flags_) { id = ID++; }

LogicalVA::State::~State() {
  // We remove every transition coming out and coming into the state.
  for (auto &f : filters)
    f->next->backward_filters_.remove(f);
  for (auto &c : captures)
    c->next->backward_captures_.remove(c);
  for (auto &f : backward_filters_)
    f->from->filters.remove(f);
  for (auto &c : backward_captures_)
    c->from->captures.remove(c);
}

bool LogicalVA::State::operator==(const State &rhs) const { return id == rhs.id; }

void LogicalVA::State::set_accepting(bool b) {
  if (b)
    flags_ |= kFinalState;
  else
    flags_ &= ~kFinalState;
}

void LogicalVA::State::set_initial(bool b) {
  if (b)
    flags_ |= kInitialState;
  else
    flags_ &= ~kInitialState;
}

void LogicalVA::State::add_capture(std::bitset<32> code, State *next) {
  for (auto const &capture : this->captures) {
    if (capture->code == code && capture->next == next)
      return;
  }

  auto sp = std::make_shared<LogicalVA::Capture>(this, code, next);
  captures.push_back(sp);
  next->backward_captures_.push_back(sp);
}

void LogicalVA::State::add_filter(unsigned int code, State *next) {
  for (auto const &filter : this->filters)
    if (filter->code == code && filter->next == next)
      return;

  auto sp = std::make_shared<LogicalVA::Filter>(this, code, next);
  filters.push_back(sp);
  next->backward_filters_.push_back(sp);
}

void LogicalVA::State::add_epsilon(State *q) {
  for (auto const &epsilon : epsilons)
    if (epsilon->next == q)
      return;

  auto sp = std::make_shared<LogicalVA::Epsilon>(this, q);
  epsilons.push_back(sp);
  q->backward_epsilons_.push_back(sp);
}

unsigned int LogicalVA::State::ID = 0;

} // end namespace rematch
