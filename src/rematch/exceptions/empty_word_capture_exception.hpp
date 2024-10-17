#pragma once

#include <stdexcept>
#include "query_syntax_exception.hpp"

namespace REmatch {

class EmptyWordCaptureException : public QuerySyntaxException {
 public:
  explicit EmptyWordCaptureException(const std::string& message)
      : QuerySyntaxException(message) {}
};

}  // namespace REmatch
