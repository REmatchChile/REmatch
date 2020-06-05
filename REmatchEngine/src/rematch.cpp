#include "rematch.hpp"
#include "parser/parser.hpp"

namespace rematch {

RExpr::RExpr(std::string re) {
  extended_automaton_ = new ExtendedVA(regex2LVA(re));
  det_automaton_ = new DetAutomaton(*extended_automaton_);
  det_man_ = new DetManager(extended_automaton_, det_automaton_);
  full_dfa_ = false;
}

std::string RExpr::uniformGenerate(uint32_t n) {
  if(!full_dfa_) {
    det_man_->computeFullDetAutomaton();
    full_dfa_ = true;
  }

  return det_man_->uniformSample(n);
}

}

