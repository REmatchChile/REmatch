#ifndef ANCHOR_INSIDE_CAPTURE_HPP
#define ANCHOR_INSIDE_CAPTURE_HPP

#include "regex_syntax_exception.hpp"

namespace REMatch {

class AnchorInsideCaptureException : public RegexSyntaxException {
 public:
  AnchorInsideCaptureException(std::string regex, size_t char_position)
      : RegexSyntaxException("Anchor found inside capture.", regex, char_position) {}
};
}  // namespace REMatch

#endif
