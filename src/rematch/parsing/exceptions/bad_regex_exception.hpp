#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <stdexcept>
#include <string>

namespace REMatch {

class BadRegexException : public std::logic_error {
 public:
  BadRegexException(std::string error) : logic_error(error), error_(error) {}
  virtual const char* what() const noexcept { return error_.c_str(); }

 protected:
  std::string error_;
};

class ParsingError : public BadRegexException {
 public:
  ParsingError(std::string error) : BadRegexException(error) {}
};

class UnhandledExpressionError : public BadRegexException {
 public:
  UnhandledExpressionError(std::string error) : BadRegexException(error) {}
};

class InvalidCharacterError : public BadRegexException {
 public:
  InvalidCharacterError(std::string error) : BadRegexException(error) {}
};

}  // namespace REMatch

#endif
