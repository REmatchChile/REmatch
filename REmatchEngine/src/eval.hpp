#ifndef EVAL_HPP
#define EVAL_HPP

#include <string>
#include <vector>
#include <memory>

#include "regex/regex.hpp"
#include "match.hpp"
#include "enumeration.hpp"
#include "automata/detautomaton.hpp"
#include "memmanager.hpp"
#include "document.hpp"
#include "automata/transition.hpp"

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

  Evaluator(RegEx& rgx, std::istream& input, uint8_t eval_options=0);
  Evaluator(RegEx& rgx, const std::string& input, uint8_t eval_options=0);

  Match_ptr next();

 private:

  void init();

  Match_ptr nextTT();
  Match_ptr nextTF();
  Match_ptr nextFT();
  Match_ptr nextFF();

  void visitEmpty(int64_t &i, Transition *t, NodeList *prev_list);
  void visitDirect(int64_t &i, Transition *t, NodeList *prev_list);
  void visitSingleCapture(int64_t &i, Transition *t, NodeList *prev_list);
  void visitDirectSingleCapture(int64_t &i, Transition *t, NodeList *prev_list);
  void visitMultiCapture(int64_t &i, Transition *t, NodeList *prev_list);
  void visitDirectMultiCapture(int64_t &i, Transition *t, NodeList *prev_list);

  void captureT(int64_t i);
  void captureF(int64_t i);

  void readingT(char a, int64_t i);
  void readingF(char a, int64_t i);

  inline Match_ptr inlinedNext(bool early_output, bool line_by_line);

  inline void capture(int64_t i, bool early_output);
  inline void reading(char a, int64_t i, bool early_output);

  void initAutomaton(int64_t i);

  bool match();

  DetAutomaton& rawDFA() {return rgx_.rawDetManager().DFA();}
  DetAutomaton& DFA() {return rgx_.detManager().DFA();}

  RegEx &rgx_;
  std::unique_ptr<Enumerator> enumerator_;
  static MemManager memory_manager_;

  std::unique_ptr<Document> text_;
  std::string line_;

  std::vector<DetState*> current_states_;
  std::vector<DetState*> new_states_;

  NodeList output_nodelist_;

  bool early_output_;
  bool line_by_line_;

  bool document_ended_;
  bool direct_text_;

  int64_t i_pos_;
  int64_t i_start_;
  int64_t nlines_;

  size_t capture_counter_;
  size_t reading_counter_;

  size_t direct_c_;
  size_t single_c_;
  size_t direct_single_c_;
  size_t direct_multi_c_;
  size_t multi_c_;
  size_t empty_c_;

  size_t det_c_;
}; // end class Evaluator

} // namespace rematch

#endif // EVAL_HPP