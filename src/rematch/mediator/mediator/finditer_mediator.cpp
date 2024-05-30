#include "finditer_mediator.hpp"

namespace rematch {

FinditerMediator::FinditerMediator(QueryData& query_data,
                                   std::shared_ptr<Document> document,
                                   Flags flags)
    : Mediator(query_data, document) {

  algorithm_ = std::make_unique<FinditerAlgorithm>(query_data.extended_va,
                                                   document_, flags);
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

mediator::Mapping* FinditerMediator::next() {
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

}  // namespace rematch
