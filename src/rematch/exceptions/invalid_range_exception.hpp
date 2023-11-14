#include "regex_syntax_exception.hpp"

namespace REMatch {

class InvalidRangeException : public RegexSyntaxException {
 public:
  InvalidRangeException(std::string invalid_range, std::string regex,
                        size_t position)
      : RegexSyntaxException(
            "Character Class range start code is greater than end code: " +
                invalid_range,
            regex, position) {}
};
}  // namespace REMatch
