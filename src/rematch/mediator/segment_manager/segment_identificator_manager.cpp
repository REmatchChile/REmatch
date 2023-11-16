#include "segment_identificator_manager.hpp"

namespace rematch {

SegmentIdentificatorManager::SegmentIdentificatorManager(SearchDFA& search_dfa, std::string_view document)
    : segment_identificator_(search_dfa, document) {}

std::unique_ptr<Span> SegmentIdentificatorManager::next() {
  return segment_identificator_.next();
}

}  // namespace rematch
