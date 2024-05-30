#ifndef MULTI_FINDONE_MEDIATOR_HPP
#define MULTI_FINDONE_MEDIATOR_HPP

#include "evaluation/algorithm/algorithm_class.hpp"
#include "evaluation/document.hpp"
#include "library_interface/query_data/query_data.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "output_enumeration/mapping.hpp"

namespace rematch {

class MultiFindoneMediator {
 public:
  MultiFindoneMediator(QueryData& query_data,
                       std::shared_ptr<Document> document,
                       Flags flags = Flags());

  std::unique_ptr<ExtendedMapping> next();

 private:
  void update_algorithm(Span& segment_span);

  std::shared_ptr<Document> document_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::unique_ptr<SegmentManager> segment_manager_;
  std::unique_ptr<AlgorithmClass> algorithm_;
  int number_of_variables_;

  const output_enumeration::Mapping* mapping_;

  bool has_output;
};

}  // namespace rematch

#endif
