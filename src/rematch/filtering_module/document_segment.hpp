#pragma once

#include "evaluation/document.hpp"
#include "filtering_module/segment_identificator.hpp"

namespace REmatch {

class DocumentSegment : public SegmentIdentificatorBase {
 public:
  explicit DocumentSegment(const std::shared_ptr<Document>& document)
      : document_size(document->size()) {}

  std::unique_ptr<Span> next() override {
    if (has_returned) {
      return nullptr;
    }
    has_returned = true;
    return std::make_unique<Span>(0, document_size);
  }

  uint64_t document_size;

  void set_document_indexes(Span&) override {}

  void evaluate_start_char() override {}

  bool has_returned = false;

  size_t get_search_dfa_size() const override { return 0; }

  size_t get_search_nfa_size() const override { return 0; }
};
}  // namespace REmatch
