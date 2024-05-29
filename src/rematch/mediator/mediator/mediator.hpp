#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "evaluation/algorithm/algorithm_class.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/query_data/query_data.hpp"
#include "mediator/mapping.hpp"
#include "mediator/segment_manager/line_by_line_manager.hpp"
#include "mediator/segment_manager/segment_identificator_manager.hpp"
#include "mediator/segment_manager/segment_manager.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "tracy/Tracy.hpp"

namespace rematch {
class Document;
using namespace REMatch;

class Mediator {
 public:
  Mediator(QueryData& query_data,
           std::shared_ptr<Document> document);

  virtual ~Mediator() = default;
  virtual mediator::Mapping* next() = 0;

 protected:
  std::shared_ptr<Document> document_;
  std::unique_ptr<SegmentManager> segment_manager_;
  std::unique_ptr<AlgorithmClass> algorithm_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  int number_of_variables_;
  const Mapping* mapping_;
  Flags flags_;

  mediator::Mapping* construct_user_mapping();
  void update_algorithm(Span& segment_span);

  friend class StatsCollector;
};
}  // namespace rematch

#endif
