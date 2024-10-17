#pragma once

#include <string>
#include "rematch_exception.hpp"

namespace REmatch {

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

}  // namespace REmatch
