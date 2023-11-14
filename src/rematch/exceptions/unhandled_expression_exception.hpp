#include "regex_syntax_exception.hpp"

namespace REMatch {

class UnhandledExpressionException : public RegexSyntaxException {
 public:
  UnhandledExpressionException(std::string message, std::string regex,
                           size_t position)
      : RegexSyntaxException(message, regex, position) {}
};
}  // namespace REMatch
