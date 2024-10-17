#pragma once

#include <utility>

#include "query_syntax_exception.hpp"

namespace REmatch {

class AnchorInsideCaptureException : public QuerySyntaxException {
 public:
  AnchorInsideCaptureException(std::string query, size_t char_position)
      : QuerySyntaxException("Anchor found inside capture.", std::move(query),
                             char_position) {}
};
}  // namespace REmatch
