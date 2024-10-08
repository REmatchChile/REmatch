#ifndef INVALID_RANGE_EXCEPTION_HPP
#define INVALID_RANGE_EXCEPTION_HPP

#include "query_syntax_exception.hpp"

namespace REmatch {

class InvalidRangeException : public QuerySyntaxException {
 public:
  InvalidRangeException(std::string invalid_range, std::string query,
                        size_t position)
      : QuerySyntaxException(
            "Character Class range start code is greater than end code: " +
                invalid_range,
            query, position) {}
};
}  // namespace REmatch
#endif