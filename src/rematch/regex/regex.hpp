#ifndef RGX_HPP
#define RGX_HPP

#include <memory>
#include <string>

#include "enumeration.hpp"
#include "match.hpp"
#include "memmanager.hpp"

#include "automata/nfa/eva.hpp"
#include "automata/nfa/sva.hpp"

#include "automata/dfa/dfa.hpp"
#include "automata/nfa/lva.hpp"
#include "evaluation/document/document.hpp"
#include "matchiterator.hpp"
#include "regex/regex_options.hpp"

namespace rematch {

class Evaluator;



class RegEx {

 public:
  using flags_t = uint16_t;

  RegEx(const std::string &regex, RegExOptions opt = RegExOptions());

  ~RegEx();

  // std::string uniformGenerate(uint32_t n);
  std::string getRegex();

  enum RegExFlags {
    kNoFlags       = 0,
    kMultiLine     = 1 << 0,
    kDotNL         = 1 << 1,
    kLineByLine    = 1 << 2,
    kEarlyOutput   = 1 << 3,
    kSaveAnchors   = 1 << 4,
    kSearching     = 1 << 5,
    kMacroDFA      = 1 << 6,
    kUnambiguous   = 1 << 7
  };

  // Calls the evaluator to get first
  Match_ptr find(const std::string &text);

  MatchIterator find_iter(std::shared_ptr<Document> d,
                         Anchor a = Anchor::kUnanchored);

  int varCount() const { return vfactory_->size(); }

  // Getters

  const std::string pattern() const { return pattern_; }
  std::shared_ptr<VariableFactory> vfactory() { return vfactory_; }

  const LogicalVA &logicalVA() const { return *VA_; }
  std::shared_ptr<ExtendedVA> extendedVA() const { return eVA_; }

private:
  static flags_t parseFlags(RegExOptions rgx_opt);

  const std::string pattern_;

  std::unique_ptr<LogicalVA> VA_;

  std::shared_ptr<VariableFactory> vfactory_;

  std::shared_ptr<ExtendedVA> eVA_;

  // Regex flags.
  flags_t flags_;

  // Is DFA fully computed.
  bool full_dfa_;

}; // end class Regex

} // end namespace rematch

#endif // RGX_HPP