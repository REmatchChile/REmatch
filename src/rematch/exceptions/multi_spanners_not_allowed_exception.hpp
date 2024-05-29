#ifndef MULTI_SPANNERS_NOT_ALLOWED_EXCEPTION_HPP
#define MULTI_SPANNERS_NOT_ALLOWED_EXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace REMatch {

class MultiSpannersNotAllowedException : public std::runtime_error {
 public:
  MultiSpannersNotAllowedException()
      : std::runtime_error(
            "Multi spanners are not allowed when using Query, use MultiQuery "
            "instead.") {}
};

}  // namespace REMatch

#endif
