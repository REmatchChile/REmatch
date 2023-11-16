#include "antlr4-runtime.h"
#include "exceptions/invalid_escape_exception.hpp"

#include <sstream>

namespace antlr4 {

void check_if_escapes_are_valid(std::string regex, std::vector<std::string> rule_stack, size_t char_position);

// Custom error listener for handling parsing errors throwing exceptions
// instead of continuing execution, which is the ANTLR's default behavior
class ParserErrorListener : public BaseErrorListener {
 public:
  void syntaxError(Recognizer* recognizer,
                   Token* offending_symbol,
                   size_t line,
                   size_t char_position_in_line,
                   const std::string& msg,
                   std::exception_ptr e) override {

    Parser* parser = dynamic_cast<Parser*>(recognizer);

    TokenStream* input = parser->getTokenStream();
    std::string regex = input->getText();

    std::vector<std::string> rule_stack = parser->getRuleInvocationStack();

    check_if_escapes_are_valid(regex, rule_stack, char_position_in_line);

    throw REMatch::RegexSyntaxException("Syntax error in regular expression.", regex, char_position_in_line);
  }
};

void check_if_escapes_are_valid(std::string regex, std::vector<std::string> rule_stack, size_t char_position) {
  if (rule_stack.front() == "escapes") {
    throw REMatch::InvalidEscapeException(regex, char_position);
  }
};
}  // namespace antlr4
