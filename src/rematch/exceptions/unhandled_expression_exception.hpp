#include "query_syntax_exception.hpp"

namespace REmatch {

class UnhandledExpressionException : public QuerySyntaxException {
 public:
  UnhandledExpressionException(const std::string& message,
                               const std::string& query, size_t position)
      : QuerySyntaxException(message, query, position) {}
};
}  // namespace REmatch
