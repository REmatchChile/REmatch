#ifndef EVALUATION__EOFILTER_EVALUATOR_NEW_HPP
#define EVALUATION__EOFILTER_EVALUATOR_NEW_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>

#include "evaluation/evaluator.hpp"
#include "regex/regex.hpp"
#include "match.hpp"
#include "structs/ecs/enumerator.hpp"
#include "automata/dfa/dfa.hpp"
#include "memmanager.hpp"
#include "evaluation/document/document.hpp"
#include "automata/dfa/transition.hpp"

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/macrodfa/macrostate.hpp"

namespace rematch {

class EarlyOutputFilterEvaluatorNew : public Evaluator {

 public:
  EarlyOutputFilterEvaluatorNew(RegEx& rgx, std::shared_ptr<StrDocument> d);

  virtual Match_ptr next();

 private:

  inline void reading(char a, int64_t i);

  bool dfa_search();

  inline void pass_current_outputs();
  inline void pass_outputs();

  DFA& dfa() {return rgx_.detManager().dfa();}
  DFA& rawDFA() {return rgx_.rawDetManager().dfa();}

  RegEx &rgx_;

  internal::Enumerator enumerator_;

  internal::ECS ds_;

  std::shared_ptr<StrDocument> text_;

  MacroDFA macro_dfa_;

  MacroState* current_state_;
  DetState* current_dstate_;

  static const size_t kSizeMaxOutputBuffer = 100;

  uint64_t i_pos_ = 0;
  uint64_t i_min_ = 0;
  uint64_t i_max_ = 0;

  size_t out_buf_counter = 0;
}; // end class Evaluator

} // namespace rematch

#endif // EVALUATION__EOFILTER_EVALUATOR_HPP