#include "multi_findone_mediator.hpp"

#include <utility>

#include "evaluation/algorithm/findone_algorithm.hpp"
#include "filtering_module/segment_identificator_base.hpp"

namespace REmatch {

MultiFindoneMediator::MultiFindoneMediator(
    QueryData& query_data, std::shared_ptr<Document> document,
    std::unique_ptr<SegmentIdentificatorBase> segment_identificator)
    : document_(std::move(document)),
      variable_catalog_(query_data.variable_catalog),
      segment_identificator(std::move(segment_identificator)) {

  algorithm_ = std::make_unique<FindoneAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);
}

std::unique_ptr<ExtendedMapping> MultiFindoneMediator::next() {
  this->segment_identificator->evaluate_start_char();
  std::unique_ptr<Span> segment_span = this->segment_identificator->next();

  if (segment_span == nullptr) {
    return nullptr;
  }

  algorithm_->begin(document_);
  update_algorithm(*segment_span);
  algorithm_->evaluate_start_char();

  mapping_ = algorithm_->next();
  auto extended_mapping = std::make_unique<ExtendedMapping>(*mapping_);
  return extended_mapping;
}

void MultiFindoneMediator::update_algorithm(Span& segment_span) {
  // add 1 to the max index to include the EOF character
  segment_span.second++;
  algorithm_->set_document_indexes(segment_span);
}

}  // namespace REmatch
