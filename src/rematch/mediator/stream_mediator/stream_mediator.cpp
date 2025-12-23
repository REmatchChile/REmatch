#include "stream_mediator.hpp"

#include "evaluation/algorithm/stream_algorithm.hpp"
#include "filtering_module/segment_identificator_stream.hpp"

namespace REmatch {

StreamMediator::StreamMediator(QueryData& query_data, const std::shared_ptr<Stream>& stream,
                               std::unique_ptr<SegmentIdentificatorStream> segment_identificator)
    : Mediator(query_data),
      variable_catalog_(query_data.variable_catalog),
      segment_identificator_(std::move(segment_identificator)) {

  algorithm_ = std::make_unique<StreamAlgorithm>(query_data.extended_va, stream,
                                                 query_data.max_mempool_duplications,
                                                 query_data.max_amount_of_states);

  segment_identificator_->evaluate_start_char();
  auto [span, segment_result] = segment_identificator_->next();

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

std::unique_ptr<mediator::Mapping> StreamMediator::next() {
#ifdef TRACY_ENABLE
  ZoneScopedNC("StreamMediator::next", 0x3c3836);
#endif
  if (next_is_computed_successfully()) {
    return construct_user_mapping();
  }

  return nullptr;
}

bool StreamMediator::next_is_computed_successfully() {
  while (true) {
    mapping_ = algorithm_->get_next_mapping();
    if (mapping_ != nullptr) {
      return true;
    }

    auto [span, segment_result] = segment_identificator_->next();
    if (segment_result != NoSegmentFound) {
      update_algorithm(*span, segment_result);
    } else {
      break;
    }
  }

  return false;
}

void StreamMediator::update_algorithm(Span& segment_span, SegmentResult segment_result) {
  segment_span.second++;
  algorithm_->set_document_indexes(segment_span, segment_result);
  algorithm_->initialize_algorithm();
}

}  // namespace REmatch
