#include <list>
#include <set>
#include <bitset>

#include "parsing/logical_variable_set_automaton/logical_va_state.hpp"

namespace rematch {

LogicalVACapture::LogicalVACapture(LogicalVAState* from, std::bitset<64> coding, LogicalVAState* next):
    from(from), next(next), code(coding) {}

bool LogicalVACapture :: operator==(const LogicalVACapture &rhs) const {
  return (code == rhs.code && next == rhs.next && from == rhs.from);
}

bool LogicalVACapture :: operator<(const LogicalVACapture &rhs) const {
  /* Simple ordering for being able to construct an std::set<LogicalVACapture> */
  if(code.to_ulong() != rhs.code.to_ulong()) {
    return code.to_ulong() < rhs.code.to_ulong();
  }

  else if(next != rhs.next) {
    return next < rhs.next;
  }

  return from < rhs.from;
}

LogicalVAState::LogicalVAState(): flags_(kDefaultLogicalVAState) { id = ID++; }

LogicalVAState::LogicalVAState(const LogicalVAState& s): flags_(s.flags_) { id = ID++; }

LogicalVAState::~LogicalVAState() {
  // We remove every transition coming out and coming into the state.
  for(auto &f: filters)
    f->next->backward_filters_.remove(f);
  for(auto &c: captures)
    c->next->backward_captures_.remove(c);
  for(auto &f: backward_filters_)
    f->from->filters.remove(f);
  for(auto &c: backward_captures_)
    c->from->captures.remove(c);
}

bool LogicalVAState::operator==(const LogicalVAState &rhs) const { return id == rhs.id;}

void LogicalVAState::set_accepting(bool b) {
  if(b)
    flags_ |= kFinalLogicalVAState;
  else
    flags_ &= ~kFinalLogicalVAState;

}

void LogicalVAState::set_initial(bool b) {
  if(b)
    flags_ |= kInitialLogicalVAState;
  else
    flags_ &= ~kInitialLogicalVAState;
}

void LogicalVAState::add_capture(std::bitset<64> code, LogicalVAState* next) {
  for(auto const &capture: this->captures) {
    if (capture->code == code && capture->next == next)
      return;
  }

  auto sp = std::make_shared<LogicalVACapture>(this, code, next);
  captures.push_back(sp);
  next->backward_captures_.push_back(sp);
}

void LogicalVAState::add_filter(unsigned int code, LogicalVAState* next) {
  for(auto const& filter: this->filters)
    if(filter->code == code && filter->next == next) return;

  auto sp = std::make_shared<LogicalVAFilter>(this, code, next);
  filters.push_back(sp);
  next->backward_filters_.push_back(sp);
}

void LogicalVAState::add_epsilon(LogicalVAState* q) {
  for(auto const& epsilon: epsilons)
    if(epsilon->next == q) return;

  auto sp = std::make_shared<LogicalVAEpsilon>(this, q);
  epsilons.push_back(sp);
  q->backward_epsilons_.push_back(sp);
}

unsigned int LogicalVAState::ID = 0;

} // end namespace rematch
