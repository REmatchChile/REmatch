#ifndef INVALID_ESCAPE_EXCEPTION_HPP
#define INVALID_ESCAPE_EXCEPTION_HPP

#include "regex_syntax_exception.hpp"

namespace REMatch {

class InvalidEscapeException : public RegexSyntaxException {
 public:
  InvalidEscapeException(std::string regex, size_t char_position)
      : RegexSyntaxException("Invalid escape character.", regex, char_position) {}
};
}  // namespace REMatch

#endif
