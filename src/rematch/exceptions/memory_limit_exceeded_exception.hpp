#pragma once

#include "evaluation_exception.hpp"

namespace REmatch {

class MemoryLimitExceededException : public EvaluationException {
 public:
  MemoryLimitExceededException()
      : EvaluationException(
            "Memory limit exceeded during document evaluation.") {}
};

}  // namespace REmatch
