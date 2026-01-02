#include "stream_multi_finditer_mediator.hpp"

#include <utility>

#include "evaluation/algorithm/stream_algorithm.hpp"
#include "evaluation/stream.hpp"
#include "output_enumeration/extended_mapping.hpp"
#include "utils/query_data.hpp"

namespace REmatch {

StreamMultiFinditerMediator::StreamMultiFinditerMediator(
    QueryData& query_data, std::shared_ptr<Stream> stream,
    std::unique_ptr<SegmentIdentificatorStream> segment_identificator)
    : variable_catalog_(query_data.variable_catalog),
      segment_identificator(std::move(segment_identificator)) {

  algorithm_ = std::make_unique<StreamAlgorithm>(query_data.extended_va, stream,
                                                 query_data.max_mempool_duplications,
                                                 query_data.max_amount_of_states);

  this->segment_identificator->evaluate_start_char();
  auto [span, segment_result] = this->segment_identificator->next();

  // The filtering did not find any segments
  if (segment_result == NoSegmentFound) {
    algorithm_->set_null_segment();

    // The filtering found a segment so we pass it to the algorithm
  } else {
    span->second++;
    algorithm_->set_document_indexes(*span, segment_result);
  }

  algorithm_->evaluate_start_char();
}

std::unique_ptr<ExtendedMapping> StreamMultiFinditerMediator::next() {
  if (!next_is_computed_successfully()) {
    return nullptr;
  }

  auto extended_mapping = std::make_unique<ExtendedMapping>(*mapping_);
  return extended_mapping;
}

bool StreamMultiFinditerMediator::next_is_computed_successfully() {
  while (true) {
    mapping_ = algorithm_->next();
    if (mapping_ != nullptr) {
      return true;
    }

    auto [span, segment_result] = segment_identificator->next();
    if (segment_result != NoSegmentFound) {
      update_algorithm(*span, segment_result);
    } else {
      break;
    }
  }

  return false;
}

void StreamMultiFinditerMediator::update_algorithm(Span& segment_span,
                                                   SegmentResult segment_result) {
  // add 1 to the max index to include the EOF character
  segment_span.second++;
  algorithm_->set_document_indexes(segment_span, segment_result);
  algorithm_->initialize_algorithm();
}

}  // namespace REmatch
