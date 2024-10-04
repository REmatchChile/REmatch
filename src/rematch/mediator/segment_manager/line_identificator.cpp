#include "line_identificator.hpp"

#include "evaluation/document.hpp"

namespace REmatch {

LineIdentificator::LineIdentificator(std::shared_ptr<Document> document)
    : document_(document) {}

std::unique_ptr<Span> LineIdentificator::next() {
  if (current_end_ >= document_->size()) {
    return nullptr;
  }

  size_t new_end_pos = document_->find('\n', current_end_);

  auto result_span = std::make_unique<Span>();

  if (new_end_pos != std::string::npos) {
    result_span->first = current_end_;
    result_span->second = new_end_pos + 1;

    current_start_ = current_end_;
    current_end_ = new_end_pos;
  }
  else {
    result_span->first = current_end_;
    result_span->second = document_->size();

    current_start_ = current_end_;
    current_end_ = document_->size();
  }

  current_end_++;

  return result_span;
}

}  // namespace REmatch
