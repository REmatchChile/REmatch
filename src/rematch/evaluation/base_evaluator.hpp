#ifndef EVALUATION__BASE_EVALUATOR_HPP
#define EVALUATION__BASE_EVALUATOR_HPP

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
#include "automata/nfa/uva.hpp"
#include "automata/dfa/sdfa.hpp"

#include "automata/dfa/transition.hpp"
#include "evaluation/document/document.hpp"

#include "evaluation/stats.hpp"

namespace rematch {

class BaseEvaluator : public Evaluator {

public:
  BaseEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d, Anchor a,
                EvalStats &e);

  Match_ptr next() override;

  bool is_ambiguous() const override { return eva_->is_ambiguous(); }

private:

  std::shared_ptr<ExtendedVA> eva_;

  std::unique_ptr<DFA> dfa_;        // Normal DFA

  std::unordered_map<DFA::State*, ECS::Node*> T_;
  std::unordered_map<DFA::State*, ECS::Node*> T_prim_;

  RegEx &rgx_;

  Enumerator enumerator_;

  ECS ds_; // DAG structure

  std::shared_ptr<StrDocument> text_;

  EvalStats &stats_;

  int i_pos_{0};
}; // end class Evaluator


} // namespace rematch

#endif // EVALUATION__EOFILTER_EVALUATOR_HPP
