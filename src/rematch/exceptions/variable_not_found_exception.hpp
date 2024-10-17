#pragma once

#include <string>

#include "argument_exception.hpp"

namespace REmatch {

class VariableNotFoundException : public ArgumentException {
 public:
  explicit VariableNotFoundException(const std::string& variable_name)
      : ArgumentException("Variable not found: " + variable_name) {}
};

}  // namespace REmatch
