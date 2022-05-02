#ifndef EVALUATION__NAIVE_EVALUATOR_HPP
#define EVALUATION__NAIVE_EVALUATOR_HPP

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
#include "automata/dfa/sdfa.hpp"

#include "memmanager.hpp"
#include "evaluation/document/document.hpp"
#include "automata/dfa/transition.hpp"

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/macrodfa/macrostate.hpp"

namespace rematch {

class NaiveEvaluator : public Evaluator {

 public:
  NaiveEvaluator(RegEx& rgx, std::shared_ptr<StrDocument> d, Anchor a);

  virtual Match_ptr next();

 private:

  inline void reading(char a, int64_t i);

  inline void visit_direct(DState* cstate, DState* direct, int64_t pos);
  inline void visit_capture(DState* cstate, std::bitset<32> capture, DState* to, int64_t pos);

  // Executes the evaluation phase. Returns true if there is an output to
  // enumerate but it didn't reach the end of the search interval. Returns
  // false otherwise.
  bool evaluation_phase();
  void init_evaluation_phase(int64_t init_from);

  inline void pass_current_outputs();
  inline void pass_outputs();

  std::unique_ptr<ExtendedVA> eva_;
  std::unique_ptr<SearchVA> sva_;

  std::unique_ptr<DFA> dfa_;                            // Normal DFA
  std::unique_ptr<SearchDFA> sdfa_;                     // Search DFA

  RegEx &rgx_;

  internal::Enumerator enumerator_;

  internal::ECS ds_;                                    // DAG structure

  std::shared_ptr<StrDocument> text_;

  Anchor anchor_;

  std::vector<DState*> current_states_;
  std::vector<DState*> new_states_;

  SDState* current_dstate_;

  uint64_t i_pos_ = 0;

  size_t out_buf_counter = 0;
}; // end class Evaluator

} // namespace rematch

#endif // EVALUATION__EOFILTER_EVALUATOR_HPP
