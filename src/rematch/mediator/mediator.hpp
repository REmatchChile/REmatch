#include "mapping.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "evaluation/algorithm_class.hpp"

namespace rematch {
class Mediator {
 public:
  Mediator(SearchDFA& search_dfa, ExtendedDetVA extended_det_va,
          std::shared_ptr<VariableCatalog> variable_catalog, std::string document);

  bool has_next();
  mediator::Mapping next();

 private:
  AlgorithmClass algorithm_;
  SegmentIdentificator segment_identificator_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  int number_of_variables_;
  int shift_ = 0;
  std::string document_;
  const Mapping* mapping_;

  void update_algorithm();
};
}
