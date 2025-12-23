#include "finditer_lbl_mediator.hpp"

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "evaluation/line.hpp"
#include "filtering_module/segment_checker.hpp"
#include "mediator/line_splitter.hpp"
#include "mediator/mediator.hpp"

#include "mediator/line_splitter.hpp"

using namespace REmatch;

FinditerLblMediator::FinditerLblMediator(QueryData& query_data, std::shared_ptr<Document> document_,
                                         std::unique_ptr<LineSplitterStr> line_splitter_,
                                         std::unique_ptr<SegmentChecker> segment_checker_)
    : Mediator(query_data),
      line_splitter(std::move(line_splitter_)),
      segment_checker(std::move(segment_checker_)),
      document(std::move(document_)) {

  algorithm = std::make_unique<FinditerAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);

  std::unique_ptr<Span> line = line_splitter->get_line();

  if (segment_checker->check(*line)) {
    update_algorithm(*line);
    algorithm->evaluate_start_char();
  }
}

std::unique_ptr<mediator::Mapping> FinditerLblMediator::next() {
  if (next_is_computed_successfully()) {
    auto mapping = construct_user_mapping();
    mapping->shift(line_offset);
    return mapping;
  }
  return nullptr;
}

bool FinditerLblMediator::next_is_computed_successfully() {
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

void FinditerLblMediator::update_algorithm(Span& span) {
  line_offset = span.first;
  std::string_view line = document->get_segment(span);
  algorithm->begin(std::make_shared<Line>(line));
}
