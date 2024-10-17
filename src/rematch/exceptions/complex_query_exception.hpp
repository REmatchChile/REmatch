#pragma once

#include <stdexcept>
#include <string>

#include "evaluation_exception.hpp"

namespace REmatch {

class ComplexQueryException : public EvaluationException {
 public:
  ComplexQueryException()
      : EvaluationException("Query is too complex to evaluate.") {}
};
}  // namespace REmatch
