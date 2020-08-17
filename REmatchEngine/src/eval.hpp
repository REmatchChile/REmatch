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

namespace rematch {

class Evaluator {

 public:

  friend class NoCapture;
  friend class OneCapture;
  friend class MultiCapture;
  friend class NoOneCapture;
  friend class NoMultiCapture;

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
}; // end class Evaluator

} // namespace rematch

#endif // EVAL_HPP