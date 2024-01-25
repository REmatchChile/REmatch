#ifndef VARIABLE_LIMIT_EXCEEDED_EXCEPTION_HPP
#define VARIABLE_LIMIT_EXCEEDED_EXCEPTION_HPP

#include <stdexcept>

namespace REMatch {

class VariableLimitExceededException : public std::runtime_error {
 public:
  VariableLimitExceededException()
      : std::runtime_error("Variable limit exceeded in the regular expression.") {}
};

}  // namespace REMatch

#endif
