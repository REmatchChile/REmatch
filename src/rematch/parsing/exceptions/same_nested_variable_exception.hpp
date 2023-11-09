#include "regex_syntax_exception.hpp"

namespace REMatch {

class SameNestedVariableException : public RegexSyntaxException {
 public:
  SameNestedVariableException(std::string variable_name, std::string regex,
                              size_t char_position)
      : RegexSyntaxException(
            "Nested the same variables inside assignation: " + variable_name,
            regex, char_position) {}
};
}  // namespace REMatch
