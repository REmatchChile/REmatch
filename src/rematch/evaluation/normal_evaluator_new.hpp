#ifndef EVALUATION_NORMAL_EVALUATOR_NEW_HPP
#define EVALUATION_NORMAL_EVALUATOR_NEW_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>

#include "evaluation/evaluator.hpp"
#include "regex/regex.hpp"
#include "match.hpp"
#include "automata/dfa/dfa.hpp"
#include "memmanager.hpp"
#include "evaluation/document/document.hpp"
#include "automata/dfa/transition.hpp"

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/macrodfa/macrostate.hpp"

#include "structs/ostruct.hpp"
#include "structs/ecs/enumerator.hpp"

namespace rematch {

class NormalEvaluatorNew : public Evaluator {

 public:
  NormalEvaluatorNew(RegEx& rgx, std::shared_ptr<Document> d);

  virtual Match_ptr next();

 private:

  inline void reading(char a, int64_t i);

  inline void pass_current_outputs();

  DFA& dfa() {return rgx_.detManager().dfa();}

  RegEx &rgx_;

  internal::Enumerator enumerator_;

  internal::ECS ds_;

  std::shared_ptr<Document> text_;

  MacroDFA macro_dfa_;

  MacroState* current_state_;
  std::vector<DetState*> new_states_;

  CharIterator current_char_;

  int64_t i_pos_;
}; // end class Evaluator

} // namespace rematch

#endif // EVALUATION_NORMAL_EVALUATOR_NEW_HPP