#include <stdexcept>

namespace REMatch {

class SameNestedVariableException : public std::runtime_error {
 public:
  SameNestedVariableException()
      : std::runtime_error("Nested the same variables inside assignation.") {}
};
}  // namespace REMatch
