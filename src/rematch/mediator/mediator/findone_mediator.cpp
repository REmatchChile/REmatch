#include "findone_mediator.hpp"
#include "evaluation/algorithm/findone_algorithm.hpp"

namespace rematch {

FindoneMediator::FindoneMediator(QueryData& query_data,
                                 std::shared_ptr<Document> document, Flags flags)
    : Mediator(query_data, document) {

  algorithm_ = std::make_unique<FindoneAlgorithm>(query_data.extended_va,
                                                  document_, flags);
  query_data.segment_manager_creator.set_document(document_);
  segment_manager_ = query_data.segment_manager_creator.get_segment_manager_for_checking();

  number_of_variables_ = variable_catalog_->size();

  std::unique_ptr<Span> segment_span = segment_manager_->next();

  has_output = segment_span != nullptr;
  if (has_output) {
    update_algorithm(*segment_span);
  }
}

mediator::Mapping* FindoneMediator::next() {
  if (!has_output) {
    return nullptr;
  }
  mapping_ = algorithm_->get_next_mapping();

  return construct_user_mapping();
}

}  // namespace rematch
