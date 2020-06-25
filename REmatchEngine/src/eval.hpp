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

class DetState;

namespace rematch {

class Evaluator {
 public:

  enum EvalOptions {
    kLineByLine    = 1<<2,
    kEarlyOutput   = 1<<3,
    kAllFlags      = kEarlyOutput | kLineByLine
  };

  Evaluator(RegEx& rgx, std::istream& input, uint8_t eval_options=0);
  Evaluator(RegEx& rgx, const std::string& input, uint8_t eval_options=0);

  std::unique_ptr<Match> next();

 private:

  void init();

  std::unique_ptr<Match> nextTT();
  std::unique_ptr<Match> nextTF();
  std::unique_ptr<Match> nextFT();
  std::unique_ptr<Match> nextFF();

  void captureT(size_t i);
  void captureF(size_t i);

  void readingT(char a, size_t i);
  void readingF(char a, size_t i);

  inline std::unique_ptr<Match> inlinedNext(bool early_output, bool line_by_line);

  inline void capture(size_t i, bool early_output);
  inline void reading(char a, size_t i, bool early_output);

  void initAutomaton(size_t i);

  bool match();

  DetAutomaton& rawDFA() {return rgx_.rawDetManager().DFA();}
  DetAutomaton& DFA() {return rgx_.detManager().DFA();}

  RegEx &rgx_;
  std::unique_ptr<Enumerator> enumerator_;
  MemManager memory_manager_;

  std::istream* input_stream_;
  std::string text_;

  std::vector<DetState*> current_states_;
  std::vector<DetState*> new_states_;
  std::vector<DetState*> capture_states_;

  NodeList output_nodelist_;

  bool early_output_;
  bool line_by_line_;

  bool document_ended_;

  size_t i_pos_;
  size_t i_start_;
  size_t nlines_;
}; // end class Evaluator

} // namespace rematch

#endif // EVAL_HPP