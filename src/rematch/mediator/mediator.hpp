#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include "mapping.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "evaluation/algorithm_class.hpp"
#include "mediation_subjects.hpp"
#include "line_by_line_helper.hpp"
#include "library_interface/flags.hpp"

namespace rematch {
using namespace REMatch;

class Mediator {
 public:
  Mediator(SearchDFA& search_dfa, ExtendedDetVA extended_det_va,
          std::shared_ptr<VariableCatalog> variable_catalog, std::string_view document, Flags flags = Flags());
  Mediator(MediationSubjects& mediation_subjects, std::string_view document, Flags flags = Flags());

  mediator::Mapping* next();

 private:
  SegmentIdentificator segment_identificator_;
  AlgorithmClass algorithm_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::shared_ptr<mediator::LineByLineHelper> line_by_line_helper_;
  std::string document_;
  int number_of_variables_;
  int shift_ = 0;
  int shift_line_ = 0;
  const Mapping* mapping_;
  Flags flags_;

  Span* get_next_segment();
  Span* get_next_line_segment();
  void update_algorithm(Span segment_span);
  void update_segment_identificator(Span line_span);
  bool next_is_computed_successfully();
};
}

#endif
