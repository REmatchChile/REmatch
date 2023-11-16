#ifndef LINE_BY_LINE_HPP
#define LINE_BY_LINE_HPP

#include <iostream>
#include <memory>
#include <string>
#include "output_enumeration/span.hpp"

namespace rematch {

class LineIdentificator {
 public:
  LineIdentificator(std::string_view document);
  std::unique_ptr<Span> next();

 private:
  std::string document_;
  int64_t current_start_ = 0;
  int64_t current_end_ = 0;
};

}  // namespace rematch

#endif
