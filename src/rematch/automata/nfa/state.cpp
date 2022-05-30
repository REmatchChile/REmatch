#include "state.hpp"

#include <bitset>
#include <list>
#include <set>

namespace rematch {

LVACapture::LVACapture(State *from, std::bitset<32> coding, State *next)
    : from(from), next(next), code(coding) {}

bool LVACapture ::operator==(const LVACapture &rhs) const {
  return (code == rhs.code && next == rhs.next && from == rhs.from);
}

bool LVACapture ::operator<(const LVACapture &rhs) const {
  /* Simple ordering for being able to construct an std::set<LVACapture> */
  if (code.to_ulong() != rhs.code.to_ulong()) {
    return code.to_ulong() < rhs.code.to_ulong();
  }

  else if (next != rhs.next) {
    return next < rhs.next;
  }

  return from < rhs.from;
}

State::State() : flags_(kDefaultState) { id = ID++; }

State::State(const State &s) : flags_(s.flags_) { id = ID++; }

State::~State() {
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

bool State::operator==(const State &rhs) const { return id == rhs.id; }

void State::set_accepting(bool b) {
  if (b)
    flags_ |= kFinalState;
  else
    flags_ &= ~kFinalState;
}

void State::set_initial(bool b) {
  if (b)
    flags_ |= kInitialState;
  else
    flags_ &= ~kInitialState;
}

void State::add_capture(std::bitset<32> code, State *next) {
  for (auto const &capture : this->captures) {
    if (capture->code == code && capture->next == next)
      return;
  }

  auto sp = std::make_shared<LVACapture>(this, code, next);
  captures.push_back(sp);
  next->backward_captures_.push_back(sp);
}

void State::add_filter(uint code, State *next) {
  for (auto const &filter : this->filters)
    if (filter->code == code && filter->next == next)
      return;

  auto sp = std::make_shared<LVAFilter>(this, code, next);
  filters.push_back(sp);
  next->backward_filters_.push_back(sp);
}

void State::add_epsilon(State *q) {
  for (auto const &epsilon : epsilons)
    if (epsilon->next == q)
      return;

  auto sp = std::make_shared<LVAEpsilon>(this, q);
  epsilons.push_back(sp);
  q->backward_epsilons_.push_back(sp);
}

unsigned int State::ID = 0;

} // end namespace rematch
