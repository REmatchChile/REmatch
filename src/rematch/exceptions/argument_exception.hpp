#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include "rematch_exception.hpp"

namespace REmatch {

class ArgumentException : public REmatchException {
 public:
  explicit ArgumentException(const std::string& message)
      : REmatchException(message) {}
};

}  // namespace REmatch
