#include "query_syntax_exception.hpp"

namespace REMatch {

class InvalidCharacterException : public QuerySyntaxException {
 public:
  InvalidCharacterException(std::string error) : QuerySyntaxException(error) {}
};

}  // namespace REMatch
