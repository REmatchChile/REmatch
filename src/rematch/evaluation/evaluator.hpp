#ifndef EVALUATION__EVALUATOR_HPP
#define EVALUATION__EVALUATOR_HPP

#include "match.hpp"

namespace rematch {

class Evaluator {
public:
  virtual Match_ptr next() = 0;
  virtual bool is_ambiguous() const = 0;
}; // end class evaluator

} // end namespace rematch

#include "evaluation/normal_evaluator.hpp"
// #include "evaluation/line_evaluator.hpp"
// #include "evaluation/eo_evaluator.hpp"
// #include "evaluation/eoline_evaluator.hpp"
// #include "evaluation/eofilter_evaluator.hpp"
// #include "evaluation/normal_evaluator_new.hpp"
// #include "evaluation/eofilter_evaluator_new.hpp"
#include "evaluation/segment_evaluator.hpp"
// #include "evaluation/base_evaluator.hpp"
// #include "evaluation/segment_evaluator_mdfa.hpp"

#endif // EVALUATION__EVALUATOR_HPP