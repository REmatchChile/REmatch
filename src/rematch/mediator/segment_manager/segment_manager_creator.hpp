#ifndef SEGMENT_MANAGER_CREATOR_HPP
#define SEGMENT_MANAGER_CREATOR_HPP

#include "default_segment_manager.hpp"
#include "line_by_line_manager.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "segment_identificator_manager.hpp"

namespace rematch {

class SegmentManagerCreator {
 private:
  bool lva_has_useful_anchors_ = false;
  std::unique_ptr<SearchDFA> search_dfa_ = nullptr;
  std::string document_;
  Flags flags;

 public:
  SegmentManagerCreator() {}
  SegmentManagerCreator(LogicalVA& logical_va, Flags flags = Flags())
      : lva_has_useful_anchors_(logical_va.has_useful_anchors()), flags(flags) {

    auto dfa_states_checker = DFAStateLimitChecker(flags);
    if (!lva_has_useful_anchors_)
      search_dfa_ = std::make_unique<SearchDFA>(logical_va, dfa_states_checker);
  }

  void set_document(std::string document) {
    document_ = document;
  }

  std::unique_ptr<SegmentManager> get_segment_manager() {
    if (!lva_has_useful_anchors_ && flags.line_by_line) {
      return std::make_unique<LineByLineManager>(std::move(search_dfa_), document_);
    } else if (!lva_has_useful_anchors_) {
      return std::make_unique<SegmentIdentificatorManager>(std::move(search_dfa_), document_);
    } else {
      return std::make_unique<DefaultSegmentManager>(document_);
    }
  }
};

}  // namespace rematch

#endif
