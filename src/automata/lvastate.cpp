#include <list>
#include <set>
#include <bitset>

#include "lvastate.hpp"

LVACapture :: LVACapture(LVAState* from, std::bitset<32> coding, LVAState* next):
    from(from), next(next), code(coding) {}

bool LVACapture :: operator==(const LVACapture &rhs) const {
  return (code == rhs.code && next == rhs.next && from == rhs.from);
}

bool LVACapture :: operator<(const LVACapture &rhs) const {
  /* Simple ordering for being able to construct an std::set<LVACapture> */
  if(code.to_ulong() != rhs.code.to_ulong()) {
    return code.to_ulong() < rhs.code.to_ulong();
  }

  else if(next != rhs.next) {
    return next < rhs.next;
  }

  return from < rhs.from;
}

LVAState::LVAState()
  : tempMark(false), colorMark('w'), visitedBy(0), isFinal(false),
    isInit(false), isSuperFinal(false) {
  id = ID++;
}

bool LVAState::operator==(const LVAState &rhs) const { return id == rhs.id;}


LVAState* LVAState::nextLVAState(unsigned int code) {
  for(auto &capture: c) {
    if (capture->code == code) {
      return capture->next;
    }
  }

  return nullptr;
}

void LVAState::setFinal(bool b) {
  isFinal = b;
}

void LVAState::setInitial(bool b) {
  isInit = b;
}

void LVAState::addEpsilon(LVAState* next) {e.push_back(std::make_shared<LVAEpsilon>(next));}

void LVAState::addCapture(std::bitset<32> code, LVAState* next) {
  for(auto const &capture: this->c) {
    if (capture->code == code && capture->next == next)
      return;
  }

  auto sp = std::make_shared<LVACapture>(this, code, next);
  c.push_back(sp);
  next->incidentCaptures.push_back(sp);
}

void LVAState::addFilter(unsigned int code, LVAState* next) {
  for(auto const& filter: this->f)
    if(filter->code == code && filter->next == next) return;

  auto sp = std::make_shared<LVAFilter>(this, code, next);
  f.push_back(sp);
  next->incidentFilters.push_back(sp);
}

unsigned int LVAState::ID = 0;


