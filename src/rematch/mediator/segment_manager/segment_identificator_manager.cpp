#include "segment_identificator_manager.hpp"

namespace REmatch {

SegmentIdentificatorManager::SegmentIdentificatorManager(SearchDFA& search_dfa, std::shared_ptr<Document> document)
    : segment_identificator_(search_dfa, document) {}

std::unique_ptr<Span> SegmentIdentificatorManager::next() {
  return segment_identificator_.next();
}

size_t SegmentIdentificatorManager::get_search_dfa_size() {
  return segment_identificator_.get_search_dfa_size();
}

size_t SegmentIdentificatorManager::get_search_nfa_size() {
  return segment_identificator_.get_search_nfa_size();
}

}  // namespace REmatch
