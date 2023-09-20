#include "mediator.hpp"

namespace rematch {

Mediator::Mediator(SearchDFA& search_dfa, ExtendedDetVA extended_det_va,
          std::shared_ptr<VariableCatalog> variable_catalog, std::string document) :
           segment_identificator_(search_dfa, document),
           algorithm_(extended_det_va, document),
           variable_catalog_(variable_catalog),
           document_(document) {

  number_of_variables_ = variable_catalog_->size();

  if (segment_identificator_.has_next()) {
    update_algorithm();
  }
}

mediator::Mapping Mediator::next() {
  mediator::Mapping result_mapping;
  
  for (int variable_id = 0; variable_id < number_of_variables_; variable_id++) {
    std::vector<Span> spans = mapping_->get_spans_of_variable_id(variable_id);

    std::string variable_name = variable_catalog_->get_var(variable_id);
    result_mapping.add_span(variable_name, spans.back());
  }

  result_mapping.shift_spans(shift_);
  return result_mapping;
}


bool Mediator::has_next() {

  while (true) {
    mapping_ = algorithm_.get_next_mapping();
    if (mapping_ != nullptr) {
      return true;
    }

    if (segment_identificator_.has_next()) {
      update_algorithm();
    } else {
      break;
    }
  }

  return false;
}

void Mediator::update_algorithm() {
  Span span = segment_identificator_.next();
  shift_ = span.first;

  std::string segment = document_.substr(shift_, span.second - span.first + 1);

  algorithm_.set_document(segment);
  algorithm_.initialize_algorithm();
}

}
