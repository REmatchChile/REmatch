#ifndef EVALUATION__SEGMENT_EVALUATOR_HPP
#define EVALUATION__SEGMENT_EVALUATOR_HPP

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "evaluation/evaluator.hpp"
#include "match.hpp"
#include "regex/regex.hpp"
#include "structs/ecs/enumerator.hpp"

#include "automata/dfa/dfa.hpp"
#include "automata/dfa/sdfa.hpp"

#include "automata/dfa/transition.hpp"
#include "evaluation/document/document.hpp"

#include "automata/macrodfa/macrodfa.hpp"
#include "automata/macrodfa/macrostate.hpp"

#include "evaluation/stats.hpp"

namespace rematch {

class SegmentEvaluator : public Evaluator {

public:
  SegmentEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d, Anchor a,
                   EvalStats &e);

  virtual Match_ptr next();

  virtual bool is_ambiguous() const { return eva_->is_ambiguous(); }

private:
  inline void reading(char a, int64_t i);

  inline void visit(DFA::State* ns, ECS::Node *node, int64_t pos,
                                                        bool garbage_left=true);

  bool searching_phase();
  void init_searching_phase();

  Match_ptr static_next();

  // Executes the evaluation phase. Returns true if there is an output to
  // enumerate but it didn't reach the end of the search interval. Returns
  // false otherwise.
  bool evaluation_phase();
  void init_evaluation_phase(int64_t init_from);

  inline void pass_current_outputs();
  inline void pass_outputs();

  Match_ptr normal_next();

  void update_current_static_mapping(int64_t i);

  std::function<Match_ptr()> next_;
  std::vector<int> static_capture_positions_;
  std::vector<int64_t> current_static_mapping_;

  std::unique_ptr<ExtendedVA> eva_;
  std::unique_ptr<SearchVA> sva_;

  std::unique_ptr<DFA> dfa_;        // Normal DFA
  std::unique_ptr<SearchDFA> sdfa_; // Search DFA

  RegEx &rgx_;

  Enumerator enumerator_;

  ECS ds_; // DAG structure
  ECS::Node* bottom_node_;

  std::shared_ptr<StrDocument> text_;

  Anchor anchor_;

  std::vector<DFA::State*> current_states_;
  std::vector<DFA::State*> new_states_;
  std::vector<DFA::State*> reached_final_states_;

  SDState *current_dstate_;

  ECS::Node *final_node_{nullptr};

  static const size_t kSizeMaxOutputBuffer = 100;

  uint64_t i_pos_ = 0;
  uint64_t i_src_ = 0;
  uint64_t i_min_ = 0;
  uint64_t i_max_ = 0;

  size_t out_buf_counter = 0;

  EvalStats &stats_;
}; // end class Evaluator

} // namespace rematch

#endif // EVALUATION__EOFILTER_EVALUATOR_HPP