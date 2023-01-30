#include <list>
#include <set>
#include <bitset>

#include "parsing/logical_variable_set_automaton/logical_va_state.hpp"
#include "parsing/logical_variable_set_automaton/logical_va_capture.hpp"

namespace rematch {
inline namespace parsing {

LogicalVAState::LogicalVAState(): flags(kDefaultLogicalVAState) { id = ID++; }

LogicalVAState::LogicalVAState(const LogicalVAState& s): flags(s.flags) { id = ID++; }

LogicalVAState::~LogicalVAState() {
  // We remove every transition coming out and coming into the state.
  for(auto &f: filters)
    delete f;
  for(auto &c: captures)
    delete c;
}

bool LogicalVAState::operator==(const LogicalVAState &rhs) const { return id == rhs.id;}

void LogicalVAState::set_accepting(bool b) {
  if(b)
    flags |= kFinalLogicalVAState;
  else
    flags &= ~kFinalLogicalVAState;

}

void LogicalVAState::set_initial(bool b) {
  if(b)
    flags |= kInitialLogicalVAState;
  else
    flags &= ~kInitialLogicalVAState;
}

void LogicalVAState::add_capture(std::bitset<64> code, LogicalVAState* next) {
  for(auto const &capture: this->captures) {
    if (capture->code == code && capture->next == next)
      return;
  }

  auto *capture = new LogicalVACapture(this, code, next);
  captures.push_back(capture);
  next->backward_captures_.push_back(capture);
}

void LogicalVAState::add_filter(CharClass charclass, LogicalVAState* next) {
  for(auto const& filter: this->filters)
    if(filter->charclass == charclass && filter->next == next) return;

  auto *filter = new LogicalVAFilter(this, charclass, next);
  filters.push_back(filter);
  next->backward_filters_.push_back(filter);
}

void LogicalVAState::add_epsilon(LogicalVAState* next) {
  for(auto const& epsilon: epsilons)
    if(epsilon->next == next) return;

  auto *epsilon = new LogicalVAEpsilon(this, next);
  epsilons.push_back(epsilon);
  next->backward_epsilons_.push_back(epsilon);
}

unsigned int LogicalVAState::ID = 0;

}
}
