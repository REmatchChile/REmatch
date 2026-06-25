#include "multi_finditer_mediator.hpp"

#include <utility>

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

MultiFinditerMediator::MultiFinditerMediator(
    QueryData& query_data, std::shared_ptr<Document> document,
    std::unique_ptr<SegmentIdentificatorBase> segment_identificator)
    : document_(std::move(document)),
      variable_catalog_(query_data.variable_catalog),
      segment_identificator(std::move(segment_identificator)) {

  algorithm_ = std::make_unique<FinditerAlgorithm>(
      query_data.extended_va, query_data.max_mempool_duplications, query_data.max_amount_of_states);

  this->segment_identificator->evaluate_start_char();
  std::unique_ptr<Span> segment = this->segment_identificator->next();

  if (segment != nullptr) {
    algorithm_->begin(document_);
    update_algorithm(*segment);
    algorithm_->evaluate_start_char();
  }
}

std::unique_ptr<ExtendedMapping> MultiFinditerMediator::next() {
  if (!next_is_computed_successfully()) {
    return nullptr;
  }

  auto extended_mapping = std::make_unique<ExtendedMapping>(*mapping_);
  return extended_mapping;
}

bool MultiFinditerMediator::next_is_computed_successfully() {
  while (true) {
    mapping_ = algorithm_->next();
    if (mapping_ != nullptr) {
      return true;
    }

    std::unique_ptr<Span> segment_span = segment_identificator->next();
    if (segment_span != nullptr) {
      update_algorithm(*segment_span);
    } else {
      break;
    }
  }

  return false;
}

void MultiFinditerMediator::update_algorithm(Span& segment_span) {
  // add 1 to the max index to include the EOF character
  segment_span.second++;
  algorithm_->set_document_indexes(segment_span);
  algorithm_->reset();
}

}  // namespace REmatch
