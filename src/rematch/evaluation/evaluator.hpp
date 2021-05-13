#ifndef EVALUATION__EVALUATOR_HPP
#define EVALUATION__EVALUATOR_HPP

#include "match.hpp"

namespace rematch {

class Evaluator {
 public:
  virtual Match_ptr next() = 0;
}; // end class evaluator

} // end namespace rematch

#endif // EVALUATION__EVALUATOR_HPP