#ifndef EVALUATION_EVALUATOR_HPP
#define EVALUATION_EVALUATOR_HPP

#include "match.hpp"

namespace rematch {

class Evaluator {
public:
  virtual Match_ptr next() = 0;
  virtual bool is_ambiguous() const = 0;
}; // end class evaluator

} // end namespace rematch

#endif // EVALUATION_EVALUATOR_HPP