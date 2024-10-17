#include "query_syntax_exception.hpp"

namespace REmatch {

class InvalidCharacterException : public QuerySyntaxException {
 public:
  explicit InvalidCharacterException(const std::string& message)
      : QuerySyntaxException(message) {}
};

}  // namespace REmatch
