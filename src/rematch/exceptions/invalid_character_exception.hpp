#include "regex_syntax_exception.hpp"

namespace REMatch {

class InvalidCharacterException : public RegexSyntaxException {
 public:
  InvalidCharacterException(std::string error) : RegexSyntaxException(error) {}
};

}  // namespace REMatch
