#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include "rematch_exception.hpp"

namespace REmatch {

class EvaluationException : public REmatchException {
 public:
  explicit EvaluationException(const std::string& message)
      : REmatchException(message) {}
};

}  // namespace REmatch
