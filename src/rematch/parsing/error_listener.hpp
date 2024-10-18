#include <antlr4-runtime.h>

#include <sstream>
#include <REmatch/exceptions.hpp>

namespace antlr4 {

void check_if_escapes_are_valid(const std::string& query, std::vector<std::string> rule_stack, size_t char_position);

// Custom error listener for handling parsing errors throwing exceptions
// instead of continuing execution, which is the ANTLR's default behavior
class ParserErrorListener : public BaseErrorListener {
 public:
  void syntaxError(Recognizer* recognizer,
                   Token* /* offending_symbol */,
                   size_t /* line */,
                   size_t char_position_in_line,
                   const std::string& /* msg */,
                   std::exception_ptr /* e */) override {

    Parser* parser = dynamic_cast<Parser*>(recognizer);

    TokenStream* input = parser->getTokenStream();
    std::string query = input->getText();

    std::vector<std::string> rule_stack = parser->getRuleInvocationStack();

    check_if_escapes_are_valid(query, rule_stack, char_position_in_line);

    throw REmatch::QuerySyntaxException("Syntax error in regular expression.", query, char_position_in_line);
  }
};

void check_if_escapes_are_valid(const std::string& query, std::vector<std::string> rule_stack, size_t char_position) {
  if (rule_stack.front() == "escapes") {
    throw REmatch::InvalidEscapeException(query, char_position);
  }
};
}  // namespace antlr4
