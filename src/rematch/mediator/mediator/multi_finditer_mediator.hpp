#ifndef MULTI_FINDITER_MEDIATOR_HPP
#define MULTI_FINDITER_MEDIATOR_HPP

#include "finditer_mediator.hpp"

#include "output_enumeration/extended_mapping.hpp"

#include "utils/query_data.hpp"


namespace REmatch {

class MultiFinditerMediator {
 public:
  MultiFinditerMediator(QueryData& query_data,
                        std::shared_ptr<Document> document,
                        uint_fast32_t max_mempool_duplications);

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

}  // namespace REmatch

#endif
