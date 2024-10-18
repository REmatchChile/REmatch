#ifndef DEFAULT_SEGMENT_MANAGER_HPP
#define DEFAULT_SEGMENT_MANAGER_HPP

#include "segment_manager.hpp"
#include "filtering_module/segment_identificator.hpp"

namespace REmatch {
class Document;

class DefaultSegmentManager : public SegmentManager {
 public:
  DefaultSegmentManager(std::shared_ptr<Document> document);

  // Returns the span representing the entire document
  std::unique_ptr<Span> next() override;
  size_t get_search_dfa_size() override;
  size_t get_search_nfa_size() override;

 private:
  std::shared_ptr<Document> document_;
  bool already_read_ = false;
};

}

#endif
