#include "mediator.hpp"

namespace rematch {

Mediator::Mediator(ExtendedDetVA& extended_det_va,
          std::shared_ptr<VariableCatalog> variable_catalog, SegmentManagerCreator& segment_manager_creator,
          std::string_view document, Flags flags) :
           algorithm_(extended_det_va, document, flags),
           variable_catalog_(variable_catalog),
           document_(document) {

  ZoneScoped;

  segment_manager_creator.set_document(document_);
  segment_manager_ = segment_manager_creator.get_segment_manager();

  number_of_variables_ = variable_catalog_->size();

  std::unique_ptr<Span> segment_span = segment_manager_->next();
  if (segment_span != nullptr) {
    update_algorithm(*segment_span);
  }
}

Mediator::Mediator(MediationSubjects& mediation_subjects,
                   SegmentManagerCreator& segment_manager_creator,
                   std::string_view document, Flags flags)
    : Mediator(mediation_subjects.extended_det_va,
               mediation_subjects.variable_catalog, segment_manager_creator,
               document, flags) {}

mediator::Mapping* Mediator::next() {
  ZoneScopedNC("Mediator::next", 0x008000);

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

  // -1 because of the START_CHAR that was added to the document
  result_mapping.shift_spans(shift_ - 1);
  return &result_mapping;
}

bool Mediator::next_is_computed_successfully() {

  while (true) {
    mapping_ = algorithm_.get_next_mapping();
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

void Mediator::update_algorithm(Span& segment_span) {
  shift_ = segment_span.first;

  std::string segment = document_.substr(shift_, segment_span.second - segment_span.first + 1);

  algorithm_.set_document(segment);
  algorithm_.initialize_algorithm();
}

}
