#ifndef EVALUATION__EO_EVALUATOR_HPP
#define EVALUATION__EO_EVALUATOR_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>

#include "evaluation/evaluator.hpp"
#include "regex/regex.hpp"
#include "match.hpp"
#include "enumeration.hpp"
#include "automata/dfa/dfa.hpp"
#include "memmanager.hpp"
#include "evaluation/document/document.hpp"
#include "automata/dfa/transition.hpp"

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/macrodfa/macrostate.hpp"


namespace rematch {

class EarlyOutputEvaluator : public Evaluator {

 public:
  EarlyOutputEvaluator(RegEx& rgx, std::shared_ptr<Document> d);

  virtual Match_ptr next();

 private:

  inline void reading(char a, int64_t i);

  inline void pass_outputs();
  inline void pass_current_outputs();

  DFA& dfa() {return rgx_.detManager().dfa();}

  RegEx &rgx_;
  Enumerator enumerator_;
  MemManager memory_manager_;

  std::shared_ptr<Document> text_;

  MacroDFA macro_dfa_;

  MacroState* current_state_;
  std::vector<DetState*> new_states_;

  CharIterator current_char_;

  int64_t i_pos_;
  size_t sp_count_;
}; // end class Evaluator

} // namespace rematch

#endif // EVALUATION__EO_EVALUATOR