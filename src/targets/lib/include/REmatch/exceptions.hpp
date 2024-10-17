#pragma once

#include <exception>
#include <string>

namespace REmatch {
inline namespace library_interface {

class REmatchException : public std::exception {
 public:
  explicit REmatchException(std::string message)
      : message_(std::move(message)) {}

  const char* what() const noexcept override { return message_.c_str(); }

 private:
  const std::string message_;
};

class QuerySyntaxException : public REmatchException {
 public:
  QuerySyntaxException(const std::string& message, const std::string& query,
                       size_t position)
      : REmatchException(message) {
    std::string caret_line = std::string(position, ' ') + "^";
    message_ = message + "\n" + query + "\n" + caret_line;
  }

  explicit QuerySyntaxException(const std::string& message)
      : REmatchException(message), message_(message) {}

  const char* what() const noexcept override { return message_.c_str(); }

 protected:
  std::string message_;
};

class EvaluationException : public REmatchException {
 public:
  explicit EvaluationException(const std::string& message)
      : REmatchException(message) {}
};

class AnchorInsideCaptureException : public QuerySyntaxException {
 public:
  AnchorInsideCaptureException(const std::string& query, size_t char_position)
      : QuerySyntaxException("Anchor found inside capture.", query,
                             char_position) {}
};

class ArgumentException : public REmatchException {
 public:
  explicit ArgumentException(const std::string& message)
      : REmatchException(message) {}
};

class ComplexQueryException : public EvaluationException {
 public:
  ComplexQueryException()
      : EvaluationException("Query is too complex to evaluate.") {}
};

class EmptyWordCaptureException : public QuerySyntaxException {
 public:
  explicit EmptyWordCaptureException(const std::string& message)
      : QuerySyntaxException(message) {}
};

class InvalidCharacterException : public QuerySyntaxException {
 public:
  explicit InvalidCharacterException(const std::string& message)
      : QuerySyntaxException(message) {}
};

class InvalidEscapeException : public QuerySyntaxException {
 public:
  InvalidEscapeException(const std::string& query, size_t char_position)
      : QuerySyntaxException("Invalid escape character.", query,
                             char_position) {}
};

class InvalidRangeException : public QuerySyntaxException {
 public:
  InvalidRangeException(const std::string& invalid_range,
                        const std::string& query, size_t position)
      : QuerySyntaxException(
            "Character Class range start code is greater than end code: " +
                invalid_range,
            query, position) {}
};

class MemoryLimitExceededException : public EvaluationException {
 public:
  MemoryLimitExceededException()
      : EvaluationException(
            "Memory limit exceeded during document evaluation.") {}
};

class MultiSpannersNotAllowedException : public QuerySyntaxException {
 public:
  MultiSpannersNotAllowedException()
      : QuerySyntaxException(
            "Multi spanners are not allowed when using Query, use MultiQuery "
            "instead.") {}
};

class SameNestedVariableException : public QuerySyntaxException {
 public:
  SameNestedVariableException(const std::string& variable_name,
                              const std::string& query, size_t char_position)
      : QuerySyntaxException(
            "Nested the same variables inside assignation: " + variable_name,
            query, char_position) {}
};

class UnhandledExpressionException : public QuerySyntaxException {
 public:
  UnhandledExpressionException(const std::string& message,
                               const std::string& query, size_t position)
      : QuerySyntaxException(message, query, position) {}
};

class VariableLimitExceededException : public EvaluationException {
 public:
  VariableLimitExceededException()
      : EvaluationException(
            "Variable limit exceeded in the regular expression.") {}
};

class VariableNotFoundException : public ArgumentException {
 public:
  explicit VariableNotFoundException(const std::string& variable_name)
      : ArgumentException("Variable not found: " + variable_name) {}
};

class VariableNotFoundInCatalogException : public ArgumentException {
 public:
  explicit VariableNotFoundInCatalogException(const std::string& variable_name)
      : ArgumentException("Variable not found in VariableCatalog: " +
                          variable_name) {}
};

}  // namespace library_interface
}  // namespace REmatch
