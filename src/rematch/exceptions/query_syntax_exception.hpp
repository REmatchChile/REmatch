#ifndef QUERY_SYNTAX_EXCEPTION_HPP
#define QUERY_SYNTAX_EXCEPTION_HPP

#include <exception>
#include <stdexcept>
#include <string>

namespace REMatch {

class QuerySyntaxException : public std::logic_error {
 public:
  QuerySyntaxException(std::string message, std::string query, size_t position) : std::logic_error(message) {
    std::string caret_line = std::string(position, ' ') + "^";
    error_ = message + "\n" + query + "\n" + caret_line;
  }

  QuerySyntaxException(std::string message) : std::logic_error(message), error_(message) {}

  virtual const char* what() const noexcept { return error_.c_str(); }

 protected:
  std::string error_;
};

}  // namespace REMatch

#endif
