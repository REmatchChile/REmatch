#pragma once

#include "query_syntax_exception.hpp"

namespace REmatch {

class InvalidEscapeException : public QuerySyntaxException {
 public:
  InvalidEscapeException(const std::string& query, size_t char_position)
      : QuerySyntaxException("Invalid escape character.", query, char_position) {}
};
}  // namespace REmatch
