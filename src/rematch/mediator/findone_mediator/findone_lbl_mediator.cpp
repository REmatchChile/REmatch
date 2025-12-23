#include "findone_lbl_mediator.hpp"

#include <utility>

#include "evaluation/algorithm/findone_algorithm.hpp"
#include "evaluation/line.hpp"

namespace REmatch {

FindoneLblMediator::FindoneLblMediator(QueryData& query_data, std::shared_ptr<Document> document,
                                       std::unique_ptr<LineSplitterStr> line_splitter,
                                       std::unique_ptr<SegmentChecker> segment_checker)
    : Mediator(query_data),
      line_splitter(std::move(line_splitter)),
      segment_checker(std::move(segment_checker)),
      document_(std::move(document)) {
  algorithm_ = std::make_unique<FindoneAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);
}

std::unique_ptr<mediator::Mapping> FindoneLblMediator::next() {
  std::unique_ptr<Span> line = this->line_splitter->get_line();

  while (line != nullptr) {
    if (segment_checker->check(*line)) {
      int64_t offset = line->first;
      update_algorithm(*line);
      algorithm_->evaluate_start_char();

      mapping_ = algorithm_->next();
      auto final_mapping = construct_user_mapping();
      final_mapping->shift(offset);
      return final_mapping;
    }

    line = line_splitter->get_line();
  }
  return nullptr;
}

void FindoneLblMediator::update_algorithm(Span& span) {
  std::string_view line = document_->get_segment(span);
  algorithm_->begin(std::make_shared<Line>(line));
}

}  // namespace REmatch
