#ifndef REMATCH_HPP
#define REMATCH_HPP

#include "det/detmanager.hpp"
#include "automata/eva.hpp"
#include "automata/detautomaton.hpp"
#include "memmanager.hpp"

#include <string>

namespace REmatch {

  class RExpr {

   public:
    RExpr(std::string regex_);

    std::string uniformGenerate(uint32_t n);
    std::string getRegex();

   private:
    ExtendedVA *extended_automaton_;
    DetAutomaton *det_automaton_;

    DetManager *det_man_;

    bool full_dfa_;

  }; // class RExpr

} // namespace rematch

#endif // REMATCH_HPP