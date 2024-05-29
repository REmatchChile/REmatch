#ifndef ANCHOR_INSIDE_CAPTURE_HPP
#define ANCHOR_INSIDE_CAPTURE_HPP

#include "query_syntax_exception.hpp"

namespace REMatch {

class AnchorInsideCaptureException : public QuerySyntaxException {
 public:
  AnchorInsideCaptureException(std::string query, size_t char_position)
      : QuerySyntaxException("Anchor found inside capture.", query, char_position) {}
};
}  // namespace REMatch

#endif
