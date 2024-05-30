#include "query_syntax_exception.hpp"

namespace REMatch {

class SameNestedVariableException : public QuerySyntaxException {
 public:
  SameNestedVariableException(std::string variable_name, std::string query,
                              size_t char_position)
      : QuerySyntaxException(
            "Nested the same variables inside assignation: " + variable_name,
            query, char_position) {}
};
}  // namespace REMatch
