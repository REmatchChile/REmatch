#include "multi_findone_mediator.hpp"
#include "evaluation/algorithm/findone_algorithm.hpp"

namespace rematch {

MultiFindoneMediator::MultiFindoneMediator(RegexData& regex_data,
                                           std::shared_ptr<Document> document,
                                           Flags flags)
    : document_(document), variable_catalog_(regex_data.variable_catalog) {

  algorithm_ = std::make_unique<FindoneAlgorithm>(regex_data.extended_va,
                                                  document_, flags);
  regex_data.segment_manager_creator.set_document(document_);
  segment_manager_ =
      regex_data.segment_manager_creator.get_segment_manager_for_checking();

  number_of_variables_ = variable_catalog_->size();

  std::unique_ptr<Span> segment_span = segment_manager_->next();

  has_output = segment_span != nullptr;
  if (has_output) {
    update_algorithm(*segment_span);
  }
}

std::unique_ptr<ExtendedMapping> MultiFindoneMediator::next() {
  if (!has_output) {
    return nullptr;
  }
  mapping_ = algorithm_->get_next_mapping();

  auto extended_mapping = std::make_unique<ExtendedMapping>(*mapping_);
  extended_mapping->shift_positions(-1);
  return extended_mapping;
}

void MultiFindoneMediator::update_algorithm(Span& segment_span) {
  // add 1 to the max index to include the EOF character
  segment_span.second++;
  algorithm_->set_document_indexes(segment_span);
  algorithm_->initialize_algorithm();
}

}  // namespace rematch