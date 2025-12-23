#pragma once

#include "evaluation/algorithm/findone_algorithm.hpp"
#include "evaluation/document.hpp"
#include "filtering_module/segment_checker.hpp"
#include "mediator/line_splitter.hpp"
#include "mediator/multi_mediator.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "output_enumeration/mapping.hpp"
#include "utils/query_data.hpp"

#include <REmatch/constants.hpp>

namespace REmatch {

class MultiFindoneLblMediator : public MultiMediator {
 public:
  MultiFindoneLblMediator(QueryData& query_data, std::shared_ptr<Document> document,
                          std::unique_ptr<LineSplitterStr> line_splitter,
                          std::unique_ptr<SegmentChecker> segment_checker);

  std::unique_ptr<ExtendedMapping> next() override;

 private:
  void update_algorithm(Span& segment_span);

  std::unique_ptr<LineSplitterStr> line_splitter;
  std::unique_ptr<SegmentChecker> segment_checker;

  std::shared_ptr<Document> document_;
  std::unique_ptr<FindoneAlgorithm> algorithm_;

  const output_enumeration::Mapping* mapping_ = nullptr;

  bool has_output;
};

}  // namespace REmatch
