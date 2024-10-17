#ifndef SEGMENT_MANAGER_CREATOR_HPP
#define SEGMENT_MANAGER_CREATOR_HPP


#include "default_segment_manager.hpp"
#include "line_by_line_manager.hpp"
#include "parsing/logical_variable_set_automaton/logical_va.hpp"
#include "segment_identificator_manager.hpp"
#include "evaluation/document.hpp"
#ifdef TRACY_ENABLE
#include "tracy/Tracy.hpp"
#endif
#include <REmatch/flags.hpp>

namespace REmatch {
class Document;

class SegmentManagerCreator {
 private:
  bool lva_has_useful_anchors_ = false;
  std::unique_ptr<SearchDFA> search_dfa_ = nullptr;
  std::shared_ptr<Document> document_;
  Flags flags;

 public:
  SegmentManagerCreator() {}
  SegmentManagerCreator(LogicalVA& logical_va, Flags flags, uint_fast32_t max_amount_of_states)
      : lva_has_useful_anchors_(logical_va.has_useful_anchors()), flags(flags) {
    #ifdef TRACY_ENABLE
    ZoneScoped;
    #endif
    auto dfa_states_checker = DFAStateLimitChecker(max_amount_of_states);
    search_dfa_ = std::make_unique<SearchDFA>(logical_va, dfa_states_checker);
  }

  void set_document(std::shared_ptr<Document> document) {
    document_ = document;
  }

  std::unique_ptr<SegmentManager> get_segment_manager() const {
    #ifdef TRACY_ENABLE
    ZoneScoped;
    #endif
    if (!lva_has_useful_anchors_ && (flags & Flags::LINE_BY_LINE) != Flags::NONE) {
      return std::make_unique<LineByLineManager>(*search_dfa_, document_);
    } else if (!lva_has_useful_anchors_) {
      return std::make_unique<SegmentIdentificatorManager>(*search_dfa_, document_);
    } else {
      return std::make_unique<DefaultSegmentManager>(document_);
    }
  }

  std::unique_ptr<SegmentManager> get_segment_manager_for_checking() const {
    if (!lva_has_useful_anchors_ && (flags & Flags::LINE_BY_LINE) != Flags::NONE) {
      return std::make_unique<LineByLineManager>(*search_dfa_, document_);
    } else {
      return std::make_unique<SegmentIdentificatorManager>(*search_dfa_, document_);
    }
  }
};

}  // namespace REmatch

#endif
