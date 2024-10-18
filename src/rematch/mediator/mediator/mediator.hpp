#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include "evaluation/algorithm/algorithm_class.hpp"
#include "evaluation/algorithm/finditer_algorithm.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "mediator/mapping.hpp"
#include "mediator/segment_manager/line_by_line_manager.hpp"
#include "mediator/segment_manager/segment_identificator_manager.hpp"
#include "mediator/segment_manager/segment_manager.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "utils/query_data.hpp"

namespace REmatch {
class Document;
class StatisticsCollector;
struct Statistics;
class Mediator {
 public:
  Mediator(QueryData& query_data, std::shared_ptr<Document> document);

  virtual ~Mediator() = default;
  virtual std::unique_ptr<mediator::Mapping> next() = 0;

 protected:
  std::shared_ptr<Document> document_;
  std::unique_ptr<SegmentManager> segment_manager_;
  std::unique_ptr<AlgorithmClass> algorithm_;
  std::shared_ptr<VariableCatalog> variable_catalog_;
  int number_of_variables_;
  const Mapping* mapping_;
  Flags flags_;

  std::unique_ptr<mediator::Mapping> construct_user_mapping();
  void update_algorithm(Span& segment_span);

  friend std::unique_ptr<Statistics> collect_statistics(const Mediator& mediator);
};
}  // namespace REmatch

#endif
