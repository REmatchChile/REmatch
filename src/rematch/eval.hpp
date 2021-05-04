#ifndef EVAL_HPP
#define EVAL_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>

#include "regex/regex.hpp"
#include "match.hpp"
#include "enumeration.hpp"
#include "automata/dfa/dfa.hpp"
#include "memmanager.hpp"
#include "document.hpp"
#include "automata/dfa/transition.hpp"

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/macrodfa/macrostate.hpp"

namespace rematch {

class DetState;
struct Transition;

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

  #ifdef COUNT_CURRENT_STATES
  std::string count_states_hist() const {
    std::stringstream ss;
    for(auto &item: current_states_count_) {
      ss << item.first << ',' << item.second << '\n';
    }
    return ss.str();
  }

  std::pair<size_t, size_t> max_count_states() const {
    size_t c_max = 0, k_max = 0;

    for(auto &item: current_states_count_) {
      if(item.second > c_max) {
        k_max = item.first;
        c_max = item.second;
      }
    }

    return std::make_pair(k_max, c_max);
  }
  #endif

 private:

  void init();

  inline void reading(char a, int64_t i, bool early_output);

  inline bool searchDFA();

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
  DetState* current_dstate_;
  std::vector<DetState*> new_states_;

  internal::NodeList output_nodelist_;

  bool early_output_;
  bool line_by_line_;

  bool document_ended_;
  bool bailed_early_;

  int64_t i_pos_;
  int64_t i_start_;
  int64_t i_low_end_;
  int64_t nlines_;

  static const size_t kMaxOutputBufferSize = 100;
  size_t out_buf_sz_;

  #ifdef COUNT_CURRENT_STATES
  std::map<size_t, size_t> current_states_count_;
  #endif
}; // end class Evaluator

} // namespace rematch

#endif // EVAL_HPP