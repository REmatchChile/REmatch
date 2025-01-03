#include "default_segment_manager.hpp"

#include "evaluation/document.hpp"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace REmatch {

DefaultSegmentManager::DefaultSegmentManager(std::shared_ptr<Document> document)
    : document_(document) {}

std::unique_ptr<Span> DefaultSegmentManager::next() {
  #ifdef TRACY_ENABLE
  ZoneScopedNC("DefaultSegmentManager::next", 0x800080);
  #endif

  if (already_read_)
    return nullptr;
  already_read_ = true;
  return std::make_unique<Span>(0, document_->size());
}

size_t DefaultSegmentManager::get_search_dfa_size() {
  return 0;
}

size_t DefaultSegmentManager::get_search_nfa_size() {
  return 0;
}

}  // namespace REmatch
