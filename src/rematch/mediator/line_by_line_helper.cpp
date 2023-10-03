#include "line_by_line_helper.hpp"
#include <iostream>

namespace rematch {
namespace mediator {

LineByLineHelper::LineByLineHelper(std::string_view document)
    : document_(std::string(document)) {}

Span* LineByLineHelper::next() {
  int64_t end_pos = document_.find('\n', current_end_);

  if (current_end_ >= document_.size())
    return nullptr;

  static Span result_span;

  if (end_pos != std::string::npos) {
    result_span = Span(current_end_, end_pos + 1);
    current_start_ = current_end_;
    current_end_ = end_pos;
  }
  else {
    result_span = Span(current_end_, document_.size());
    current_start_ = current_end_;
    current_end_ = document_.size();
  }

  current_end_++;

  return &result_span;
}

}  // namespace mediator
}  // namespace rematch
