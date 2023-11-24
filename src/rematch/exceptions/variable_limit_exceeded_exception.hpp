#include <stdexcept>

namespace REMatch {

class VariableLimitExceededException : public std::runtime_error {
 public:
  VariableLimitExceededException()
      : std::runtime_error("Variable limit exceeded in the regular expression.") {}
};

}  // namespace REMatch
