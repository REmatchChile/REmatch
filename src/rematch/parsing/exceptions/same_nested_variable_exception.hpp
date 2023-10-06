#include <stdexcept>

namespace REMatch {

class SameNestedVariableException : public std::runtime_error {
 public:
  SameNestedVariableException(std::string variable_name)
      : std::runtime_error("Nested the same variables inside assignation: " + variable_name) {}
};
}  // namespace REMatch
