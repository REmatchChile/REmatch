#ifndef RGX_HPP
#define RGX_HPP

#include <string>

#include "match.hpp"
#include "memmanager.hpp"
#include "enumeration.hpp"

#include "det/detmanager.hpp"
#include "automata/eva.hpp"
#include "automata/detautomaton.hpp"
#include "regex/regex_options.hpp"
#include "automata/lva.hpp"
#include "evaliter.hpp"

namespace rematch {

class RegEx {

 public:

  using flags_t = uint8_t;

  RegEx(std::string regex, RegExOptions opt = RegExOptions());

  ~RegEx();

  std::string uniformGenerate(uint32_t n);
  std::string getRegex();

  enum RegExFlags {
    kNoFlags     = 0,
    kMultiLine   = 1<<0,
    kDotNL       = 1<<1,
    kLineByLine  = 1<<2,
    kEarlyOutput = 1<<3,
    kSaveAnchors = 1<<4
  };

  // Calls the evaluator to get first
  Match_ptr find(const std::string &text);

  EvaluatorIter findIter(const std::string &text);
  EvaluatorIter findIterFile(std::istream& is);

  int varCount() const {return dman_.varFactory()->size();}

  std::vector<std::string> varScheme() const {return dman_.varFactory()->getOutputSchema();}

  // Getters

  const std::string pattern() const {return pattern_;}

  // Detmanager stuff.

  DetManager& detManager() {return dman_;};
  DetManager& rawDetManager() {return raw_dman_;};

  // Stats passed to interface

  private:

  static flags_t parseFlags(RegExOptions rgx_opt);

  const std::string pattern_;

  // DetManager for capture automaton.
  DetManager dman_;

  // DetManager for raw automaton.
  DetManager raw_dman_;

  // Regex flags.
  flags_t flags_;

  Anchor anchors_;

  // Is DFA fully computed.
  bool full_dfa_;

}; // end class Regex

} // end namespace rematch




#endif // RGX_HPP