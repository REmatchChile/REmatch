#include "mediator.hpp"
#include "evaluation/algorithm/finditer_algorithm.hpp"

#include "evaluation/document.hpp"

namespace rematch {

Mediator::Mediator(QueryData& query_data, std::shared_ptr<Document> document)
    : document_(document), variable_catalog_(query_data.variable_catalog) {}

mediator::Mapping* Mediator::construct_user_mapping() {
  static mediator::Mapping result_mapping;
  result_mapping.reset();

  std::map<int, std::vector<Span>> spans_map = mapping_->construct_mapping();

  for (int variable_id = 0; variable_id < number_of_variables_; variable_id++) {
    Span span = spans_map[variable_id].back();

    std::string variable_name = variable_catalog_->get_var(variable_id);
    result_mapping.add_span(variable_name, span);
  }

  // -1 because of the START_CHAR that was added to the document
  result_mapping.shift_spans(-1);
  return &result_mapping;
}

void Mediator::update_algorithm(Span& segment_span) {
  // add 1 to the max index to include the EOF character
  segment_span.second++;
  algorithm_->set_document_indexes(segment_span);
  algorithm_->initialize_algorithm();
}

}  // namespace rematch
