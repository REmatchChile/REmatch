#pragma once

#include "query_syntax_exception.hpp"

namespace REmatch {

class InvalidRangeException : public QuerySyntaxException {
 public:
  InvalidRangeException(const std::string& invalid_range,
                        const std::string& query, size_t position)
      : QuerySyntaxException(
            "Character Class range start code is greater than end code: " +
                invalid_range,
            query, position) {}
};
}  // namespace REmatch
