#ifndef LINE_BY_LINE_HPP
#define LINE_BY_LINE_HPP

#include <string>
#include "output_enumeration/span.hpp"

namespace rematch {
namespace mediator {

class LineByLineHelper {
 public:
  LineByLineHelper(std::string_view document);
  Span* next();

 private:
  std::string document_;
  int64_t current_start_ = -1;
  int64_t current_end_ = 0;
};

}  // namespace mediator
}  // namespace rematch

#endif
