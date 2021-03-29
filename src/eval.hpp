#ifndef EVAL_HPP
#define EVAL_HPP

#include <string>
#include <vector>
#include <memory>

#include "regex/regex.hpp"
#include "match.hpp"
#include "enumeration.hpp"
#include "automata/dfa/dfa.hpp"
#include "memmanager.hpp"
#include "document.hpp"
#include "automata/dfa/transition.hpp"

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/macrodfa/macrostate.hpp"

class DetState;
struct Transition;

namespace rematch {

class Evaluator {

 public:

  friend class RegEx;

  enum EvalOptions {
    kLineByLine    = 1<<2,
    kEarlyOutput   = 1<<3,
    kAllFlags      = kEarlyOutput | kLineByLine
  };

  Evaluator(RegEx& rgx, std::shared_ptr<Document> d, uint8_t eval_options=0);

  Match_ptr next();

 private:

  void init();

  void visitEmpty(int64_t i, Transition *t, NodeList *prev_list);
  void visitDirect(int64_t i, Transition *t, NodeList *prev_list);
  void visitSingleCapture(int64_t i, Transition *t, NodeList *prev_list);
  void visitDirectSingleCapture(int64_t i, Transition *t, NodeList *prev_list);
  void visitMultiCapture(int64_t i, Transition *t, NodeList *prev_list);
  void visitDirectMultiCapture(int64_t i, Transition *t, NodeList *prev_list);

  inline void capture(int64_t i, bool early_output);
  inline void reading(char a, int64_t i, bool early_output);

  void initAutomaton(int64_t i);

  bool match();

  DFA& rawDFA() {return rgx_.rawDetManager().dfa();}
  DFA& dfa() {return rgx_.detManager().dfa();}

  RegEx &rgx_;
  std::unique_ptr<Enumerator> enumerator_;
  static MemManager memory_manager_;

  std::shared_ptr<Document> text_;
  std::string line_;

  MacroDFA macro_dfa_;

  MacroState* current_state_;
  std::vector<DetState*> new_states_;

  NodeList output_nodelist_;

  bool early_output_;
  bool line_by_line_;

  bool document_ended_;
  bool direct_text_;

  int64_t i_pos_;
  int64_t i_start_;
  int64_t nlines_;
}; // end class Evaluator

} // namespace rematch

#endif // EVAL_HPP