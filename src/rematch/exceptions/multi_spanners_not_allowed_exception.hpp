#pragma once

#include <string>

#include "query_syntax_exception.hpp"

namespace REmatch {

class MultiSpannersNotAllowedException : public QuerySyntaxException {
 public:
  MultiSpannersNotAllowedException()
      : QuerySyntaxException(
            "Multi spanners are not allowed when using Query, use MultiQuery "
            "instead.") {}
};

}  // namespace REmatch
