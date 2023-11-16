#include "line_by_line_manager.hpp"

namespace rematch {

LineByLineManager::LineByLineManager(SearchDFA& search_dfa,
                                     std::string_view document)
    : segment_identificator_(search_dfa, document),
      line_identificator_(document),
      document_(document) {
  std::unique_ptr<Span> line_span = line_identificator_.next();
  if (line_span != nullptr) {
    update_line_in_segment_identificator(*line_span);
  }
}

std::unique_ptr<Span> LineByLineManager::next() {
  while (true) {
    std::unique_ptr<Span> segment = segment_identificator_.next();
    if (segment != nullptr) {
      segment->first += shift_;
      segment->second += shift_;
      return segment;
    }

    std::unique_ptr<Span> line_span = line_identificator_.next();
    if (line_span != nullptr) {
      update_line_in_segment_identificator(*line_span);
    } else {
      break;
    }
  }
  return nullptr;
}

void LineByLineManager::update_line_in_segment_identificator(Span& line_span) {
  shift_ = line_span.first;
  std::string line =
      document_.substr(line_span.first, line_span.second - line_span.first);
  segment_identificator_.set_document(line);
}

};  // namespace rematch
