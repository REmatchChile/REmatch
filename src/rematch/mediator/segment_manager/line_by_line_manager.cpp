#include "line_by_line_manager.hpp"

#include "evaluation/document.hpp"

namespace REmatch {

LineByLineManager::LineByLineManager(SearchDFA& search_dfa,
                                     std::shared_ptr<Document> document)
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
  segment_identificator_.set_document_indexes(line_span);
}

size_t LineByLineManager::get_search_dfa_size() {
  return segment_identificator_.get_search_dfa_size();
}

size_t LineByLineManager::get_search_nfa_size() {
  return segment_identificator_.get_search_nfa_size();
}

};  // namespace REmatch
