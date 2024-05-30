#ifndef MULTI_FINDITER_MEDIATOR_HPP
#define MULTI_FINDITER_MEDIATOR_HPP

#include "finditer_mediator.hpp"
#include "output_enumeration/extended_mapping.hpp"

namespace rematch {

class MultiFinditerMediator {
 public:
  MultiFinditerMediator(QueryData& query_data,
                        std::shared_ptr<Document> document,
                        Flags flags = Flags());

  std::unique_ptr<ExtendedMapping> next();

 private:
  bool next_is_computed_successfully();
  void update_algorithm(Span& segment_span);

  std::shared_ptr<Document> document_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  std::unique_ptr<SegmentManager> segment_manager_;
  std::unique_ptr<AlgorithmClass> algorithm_;
  int number_of_variables_;

  const output_enumeration::Mapping* mapping_;
};

}  // namespace rematch

#endif
