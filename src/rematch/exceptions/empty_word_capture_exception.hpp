#ifndef EMPTY_WORD_CAPTURE_EXCEPTION_HPP
#define EMPTY_WORD_CAPTURE_EXCEPTION_HPP

#include <stdexcept>

namespace REMatch {

class EmptyWordCaptureException : public std::runtime_error {
 public:
  EmptyWordCaptureException(const std::string& error)
      : std::runtime_error(error) {}
};

}  // namespace REMatch

#endif
