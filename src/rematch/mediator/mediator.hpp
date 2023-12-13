#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include "mapping.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "evaluation/algorithm_class.hpp"
#include "mediation_subjects.hpp"
#include "library_interface/flags.hpp"
#include "segment_manager/segment_manager.hpp"
#include "segment_manager/segment_identificator_manager.hpp"
#include "segment_manager/line_by_line_manager.hpp"
#include "segment_manager/segment_manager_creator.hpp"

namespace rematch {
using namespace REMatch;

class Mediator {
 public:
  Mediator(ExtendedDetVA& extended_det_va,
           std::shared_ptr<VariableCatalog> variable_catalog,
           SegmentManagerCreator& segment_manager_creator,
           std::string_view document, Flags flags = Flags());
  Mediator(MediationSubjects& mediation_subjects,
           SegmentManagerCreator& segment_manager_creator,
           std::string_view document, Flags flags = Flags());

  mediator::Mapping* next();

 private:
  std::unique_ptr<SegmentManager> segment_manager_;
  AlgorithmClass algorithm_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::string document_;
  int number_of_variables_;
  int shift_ = 0;
  const Mapping* mapping_;
  Flags flags_;

  void update_algorithm(Span& segment_span);
  bool next_is_computed_successfully();

  friend class StatsCollector;
};
}

#endif
