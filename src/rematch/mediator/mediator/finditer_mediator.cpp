#include "finditer_mediator.hpp"

namespace REmatch {

FinditerMediator::FinditerMediator(QueryData& query_data,
                                   std::shared_ptr<Document> document,
                                   uint_fast32_t max_mempool_duplications,
                                   uint_fast32_t max_deterministic_states)
    : Mediator(query_data, document) {

  algorithm_ = std::make_unique<FinditerAlgorithm>(
      query_data.extended_va, document_, max_mempool_duplications,
      max_deterministic_states);
  query_data.segment_manager_creator.set_document(document_);
  segment_manager_ = query_data.segment_manager_creator.get_segment_manager();

  number_of_variables_ = variable_catalog_->size();

  std::unique_ptr<Span> segment_span = segment_manager_->next();
  if (segment_span != nullptr) {
    update_algorithm(*segment_span);
  } else {
    algorithm_->set_null_segment();
  }
}

std::unique_ptr<mediator::Mapping> FinditerMediator::next() {
  if (!next_is_computed_successfully()) {
    return nullptr;
  }
  return construct_user_mapping();
}

bool FinditerMediator::next_is_computed_successfully() {
  while (true) {
    mapping_ = algorithm_->get_next_mapping();
    if (mapping_ != nullptr) {
      return true;
    }

    std::unique_ptr<Span> segment_span = segment_manager_->next();
    if (segment_span != nullptr) {
      update_algorithm(*segment_span);
    } else {
      break;
    }
  }

  return false;
}

}  // namespace REmatch
