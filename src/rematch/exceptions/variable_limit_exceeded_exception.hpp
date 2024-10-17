#pragma once

#include "evaluation_exception.hpp"

namespace REmatch {

class VariableLimitExceededException : public EvaluationException {
 public:
  VariableLimitExceededException()
      : EvaluationException(
            "Variable limit exceeded in the regular expression.") {}
};

}  // namespace REmatch
