#include "multi_finditer_lbl_mediator.hpp"

#include <utility>

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "evaluation/line.hpp"
#include "filtering_module/segment_checker.hpp"
#include "mediator/line_splitter.hpp"
#include "mediator/mediator.hpp"

using namespace REmatch;

MultiFinditerLblMediator::MultiFinditerLblMediator(QueryData& query_data,
                                                   std::shared_ptr<Document> document,
                                                   std::unique_ptr<LineSplitterStr> line_splitter,
                                                   std::unique_ptr<SegmentChecker> segment_checker)
    : line_splitter(std::move(line_splitter)),
      segment_checker(std::move(segment_checker)),
      document_(std::move(document)) {

  algorithm = std::make_unique<FinditerAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);

  std::unique_ptr<Span> line = this->line_splitter->get_line();

  if (this->segment_checker->check(*line)) {
    update_algorithm(*line);
    algorithm->evaluate_start_char();
  }
}

std::unique_ptr<ExtendedMapping> MultiFinditerLblMediator::next() {
  if (!next_is_computed_successfully()) {
    return nullptr;
  }
  auto extended_mapping = std::make_unique<ExtendedMapping>(*mapping_);
  extended_mapping->shift(line_offset);
  return extended_mapping;
}

bool MultiFinditerLblMediator::next_is_computed_successfully() {
  while (keep) {
    mapping_ = algorithm->next();
    if (mapping_ != nullptr) {
      return true;
    }

    keep = false;
    while (std::unique_ptr<Span> line = line_splitter->get_line()) {
      if (segment_checker->check(*line)) {
        update_algorithm(*line);
        algorithm->evaluate_start_char();
        keep = true;
        break;
      }
    }
  }

  return false;
}

void MultiFinditerLblMediator::update_algorithm(Span& span) {
  line_offset = span.first;
  std::string_view line = document_->get_segment(span);
  algorithm->begin(std::make_shared<Line>(line));
}
