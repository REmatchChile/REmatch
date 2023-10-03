#include "mediator.hpp"

namespace rematch {

Mediator::Mediator(SearchDFA& search_dfa, ExtendedDetVA extended_det_va,
          std::shared_ptr<VariableCatalog> variable_catalog, std::string_view document, Flags flags) :
           segment_identificator_(search_dfa, document),
           algorithm_(extended_det_va, document),
           variable_catalog_(variable_catalog),
           line_by_line_helper_(std::make_shared<mediator::LineByLineHelper>(document)),
           document_(document),
           flags_(flags) {

  number_of_variables_ = variable_catalog_->size();

  if (flags.line_by_line) {
    Span* line_span = line_by_line_helper_->next();
    if (line_span != nullptr) {
      update_segment_identificator(*line_span);
    }
  }

  Span* segment_span = segment_identificator_.next();
  if (segment_span != nullptr) {
    update_algorithm(*segment_span);
  }
}

Mediator::Mediator(MediationSubjects& mediation_subjects, std::string_view document, Flags flags) :
      Mediator(mediation_subjects.search_dfa,
               mediation_subjects.extended_det_va,
               mediation_subjects.variable_catalog, document, flags) {}

mediator::Mapping* Mediator::next() {
  if (!next_is_computed_successfully()) {
    return nullptr;
  }

  static mediator::Mapping result_mapping;
  result_mapping.reset();

  for (int variable_id = 0; variable_id < number_of_variables_; variable_id++) {
    std::vector<Span> spans = mapping_->get_spans_of_variable_id(variable_id);

    std::string variable_name = variable_catalog_->get_var(variable_id);
    result_mapping.add_span(variable_name, spans.back());
  }

  result_mapping.shift_spans(shift_);
  return &result_mapping;
}

bool Mediator::next_is_computed_successfully() {

  while (true) {
    mapping_ = algorithm_.get_next_mapping();
    if (mapping_ != nullptr) {
      return true;
    }

    Span* segment_span = get_next_segment();
    if (segment_span != nullptr) {
      update_algorithm(*segment_span);
    } else {
      break;
    }
  }

  return false;
}

Span* Mediator::get_next_segment() {
  if (flags_.line_by_line)
    return get_next_line_segment();
  return segment_identificator_.next();
}

Span* Mediator::get_next_line_segment() {
  while (true) {
    Span* segment_span = segment_identificator_.next();
    if (segment_span != nullptr) {
      return segment_span;
    }

    Span* line_segment = line_by_line_helper_->next();
    if (line_segment != nullptr) {
      update_segment_identificator(*line_segment);
    } else {
      break;
    }
  }
  return nullptr;
}

void Mediator::update_algorithm(Span segment_span) {
  shift_ = segment_span.first + shift_line_;

  std::string segment = document_.substr(shift_, segment_span.second - segment_span.first + 1);

  algorithm_.set_document(segment);
  algorithm_.initialize_algorithm();
}

void Mediator::update_segment_identificator(Span line_span) {
  shift_line_ = line_span.first;
  std::string segment = document_.substr(line_span.first, line_span.second - line_span.first);

  segment_identificator_.set_document(segment);
}

}
