#include "findone_mediator.hpp"

#include <utility>

#include "evaluation/algorithm/findone_algorithm.hpp"
#include "evaluation/line.hpp"
#include "filtering_module/segment_identificator_base.hpp"

namespace REmatch {

FindoneMediator::FindoneMediator(QueryData& query_data, std::shared_ptr<Document> document,
                                 std::unique_ptr<SegmentIdentificatorBase> segment_identificator)
    : Mediator(query_data),
      segment_identificator(std::move(segment_identificator)),
      document_(std::move(document)) {

  algorithm_ = std::make_unique<FindoneAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);
}

std::unique_ptr<mediator::Mapping> FindoneMediator::next() {

  segment_identificator->evaluate_start_char();
  std::unique_ptr<Span> segment_span = segment_identificator->next();

  if (segment_span == nullptr) {
    return nullptr;
  }

  algorithm_->begin(document_);
  update_algorithm(*segment_span);
  algorithm_->evaluate_start_char();

  mapping_ = algorithm_->next();
  return construct_user_mapping();
}

void FindoneMediator::update_algorithm(Span& span) {
  // add 1 to the max index to include the EOF character
  span.second++;
  algorithm_->set_document_indexes(span);
}

}  // namespace REmatch
