#include <stdexcept>

namespace REMatch {

class VariableNotFoundInCatalogException : public std::runtime_error {
 public:
  VariableNotFoundInCatalogException(const std::string& variable_name)
      : std::runtime_error("Variable not found in VariableCatalog: " + variable_name) {}
};
}  // namespace REMatch
