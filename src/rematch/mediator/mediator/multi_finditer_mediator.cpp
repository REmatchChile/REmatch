#include "multi_finditer_mediator.hpp"

#include "output_enumeration/extended_mapping.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

MultiFinditerMediator::MultiFinditerMediator(QueryData& query_data,
                                             std::shared_ptr<Document> document,
                                             uint_fast32_t max_mempool_duplications,
                                             uint_fast32_t max_deterministic_states)
    : document_(document), variable_catalog_(query_data.variable_catalog) {

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

std::unique_ptr<ExtendedMapping> MultiFinditerMediator::next() {
  if (!next_is_computed_successfully()) {
    return nullptr;
  }

  auto extended_mapping = std::make_unique<ExtendedMapping>(*mapping_);
  extended_mapping->shift_positions(-1);
  return extended_mapping;
}

bool MultiFinditerMediator::next_is_computed_successfully() {
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

void MultiFinditerMediator::update_algorithm(Span& segment_span) {
  // add 1 to the max index to include the EOF character
  segment_span.second++;
  algorithm_->set_document_indexes(segment_span);
  algorithm_->initialize_algorithm();
}

}  // namespace REmatch
