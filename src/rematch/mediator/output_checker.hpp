#ifndef OUTPUT_CHECKER_HPP
#define OUTPUT_CHECKER_HPP

#include "library_interface/query_data/query_data.hpp"

namespace rematch {

class OutputChecker {
 public:
  OutputChecker(QueryData& query_data, std::shared_ptr<Document> document)
      : document_(document) {
    query_data.segment_manager_creator.set_document(document_);
    segment_manager_ =
        query_data.segment_manager_creator.get_segment_manager_for_checking();
  }

  bool check() { return segment_manager_->next() != nullptr; }

 private:
  std::unique_ptr<SegmentManager> segment_manager_;
  std::shared_ptr<Document> document_;
};

}  // namespace rematch

#endif
