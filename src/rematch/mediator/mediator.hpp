#include "mapping.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "evaluation/algorithm_class.hpp"

namespace rematch {
class Mediator {
 public:
  Mediator(SearchDFA& search_dfa, ExtendedDetVA extended_det_va,
          std::shared_ptr<VariableCatalog> variable_catalog, std::string& document);

  mediator::Mapping* next();

 private:
  SegmentIdentificator segment_identificator_;
  AlgorithmClass algorithm_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::string document_;
  int number_of_variables_;
  int shift_ = 0;
  const Mapping* mapping_;

  void update_algorithm(Span segment_span);
  bool next_is_computed_successfully();
};
}
