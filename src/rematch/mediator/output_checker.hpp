#ifndef OUTPUT_CHECKER_HPP
#define OUTPUT_CHECKER_HPP

#include "library_interface/regex_data/regex_data.hpp"

namespace rematch {

class OutputChecker {
 public:
  OutputChecker(RegexData& regex_data, std::string_view document, Flags flags)
      : document_(document) {
    regex_data.segment_manager_creator.set_document(document_);
    segment_manager_ =
        regex_data.segment_manager_creator.get_segment_manager_for_checking();
  }

  bool check() { return segment_manager_->next() != nullptr; }

 private:
  std::unique_ptr<SegmentManager> segment_manager_;
  std::string_view document_;
};

}  // namespace rematch

#endif
