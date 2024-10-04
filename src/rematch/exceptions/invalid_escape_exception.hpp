#ifndef INVALID_ESCAPE_EXCEPTION_HPP
#define INVALID_ESCAPE_EXCEPTION_HPP

#include "query_syntax_exception.hpp"

namespace REmatch {

class InvalidEscapeException : public QuerySyntaxException {
 public:
  InvalidEscapeException(std::string query, size_t char_position)
      : QuerySyntaxException("Invalid escape character.", query, char_position) {}
};
}  // namespace REmatch

#endif
