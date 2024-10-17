#pragma once

#include "argument_exception.hpp"

namespace REmatch {

class VariableNotFoundInCatalogException : public ArgumentException {
 public:
  explicit VariableNotFoundInCatalogException(const std::string& variable_name)
      : ArgumentException("Variable not found in VariableCatalog: " +
                          variable_name) {}
};
}  // namespace REmatch
