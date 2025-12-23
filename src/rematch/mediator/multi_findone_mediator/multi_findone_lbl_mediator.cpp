#include "multi_findone_lbl_mediator.hpp"

#include <utility>

#include "evaluation/algorithm/findone_algorithm.hpp"
#include "evaluation/line.hpp"

namespace REmatch {

MultiFindoneLblMediator::MultiFindoneLblMediator(QueryData& query_data,
                                                 std::shared_ptr<Document> document,
                                                 std::unique_ptr<LineSplitterStr> line_splitter,
                                                 std::unique_ptr<SegmentChecker> segment_checker)
    : line_splitter(std::move(line_splitter)),
      segment_checker(std::move(segment_checker)),
      document_(std::move(document)) {

  algorithm_ = std::make_unique<FindoneAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);

  std::unique_ptr<Span> line = this->line_splitter->get_line();

  if (line != nullptr && this->segment_checker->check(*line)) {
    has_output = true;
    update_algorithm(*line);
  }
}

std::unique_ptr<ExtendedMapping> MultiFindoneLblMediator::next() {
  std::unique_ptr<Span> line = this->line_splitter->get_line();

  while (line != nullptr) {
    if (segment_checker->check(*line)) {
      int64_t offset = line->first;
      update_algorithm(*line);
      algorithm_->evaluate_start_char();

      mapping_ = algorithm_->next();
      auto extended_mapping = std::make_unique<ExtendedMapping>(*mapping_);
      extended_mapping->shift(offset);
      return extended_mapping;
    }

    line = line_splitter->get_line();
  }
  return nullptr;
}

void MultiFindoneLblMediator::update_algorithm(Span& span) {
  std::string_view line = document_->get_segment(span);
  algorithm_->begin(std::make_shared<Line>(line));
}

}  // namespace REmatch
