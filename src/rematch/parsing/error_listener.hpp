#include "antlr4-runtime.h"

#include <sstream>

namespace antlr4 {

// Custom error listener for handling parsing errors throwing exceptions 
// instead of continuing execution, which is the ANTLR's default behavior
class ParserErrorListener : public BaseErrorListener {
public:
  void syntaxError(Recognizer * /* recognizer */,
                   Token * /* offendingSymbol */,
                   size_t line,
                   size_t charPositionInLine,
                   const std::string &msg,
                   std::exception_ptr /* e */) override {
    std::stringstream ss;
    ss << "line " << line << ":" << (charPositionInLine + 1) << " " << msg;
    throw std::runtime_error(ss.str());
  }
};
} // namespace antlr4
