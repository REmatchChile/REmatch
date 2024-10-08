#ifndef SAME_NESTED_VARIABLE_EXCEPTION_HPP
#define SAME_NESTED_VARIABLE_EXCEPTION_HPP

#include "query_syntax_exception.hpp"

namespace REmatch {

class SameNestedVariableException : public QuerySyntaxException {
 public:
  SameNestedVariableException(const std::string& variable_name,
                              const std::string& query, size_t char_position)
      : QuerySyntaxException(
            "Nested the same variables inside assignation: " + variable_name,
            query, char_position) {}
};
}  // namespace REmatch
#endif