#include "query_syntax_exception.hpp"

namespace REmatch {

class InvalidCharacterException : public QuerySyntaxException {
 public:
  InvalidCharacterException(std::string error) : QuerySyntaxException(error) {}
};

}  // namespace REmatch
