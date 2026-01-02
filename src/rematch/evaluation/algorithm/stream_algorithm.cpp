#include "stream_algorithm.hpp"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace REmatch {

StreamAlgorithm::StreamAlgorithm(ExtendedVA& extended_va,
                                 std::shared_ptr<Stream> stream,
                                 uint_fast32_t max_mempool_duplications,
                                 uint_fast32_t max_deterministic_states)
    : stream_(std::move(stream)),
      extended_det_va_(extended_va, max_deterministic_states) {
  ECS_interface_ = std::make_unique<ECS>(max_mempool_duplications);
  enumerator_ = std::make_unique<Enumerator>();

  ExtendedDetVAState* initial_state = extended_det_va_.get_initial_state();
  ECSNode* bottom_node = ECS_interface_->create_bottom_node();
  initial_state->set_node(bottom_node);
  ECS_interface_->pin_node(bottom_node);

  initialize_algorithm();
}

void StreamAlgorithm::initialize_algorithm() {
  current_states_.clear();
  next_states_.clear();
  reached_final_states_.clear();

  extended_det_va_.set_state_initial_phases();
  ExtendedDetVAState* initial_state = extended_det_va_.get_initial_state();
  current_states_.push_back(initial_state);
}

void StreamAlgorithm::set_document_indexes(Span& span,
                                           SegmentResult segment_result) {
  pos_i_ = span.first;
  end_i_ = span.second;
  this->segment_result = segment_result;
}

void StreamAlgorithm::set_null_segment() {
  pos_i_ = 0;
  end_i_ = 0;
  segment_result = NoSegmentFound;
}

const Mapping* StreamAlgorithm::next() {
#ifdef TRACY_ENABLE
  ZoneScopedNC("StreamAlgorithm::get_next_mapping", 0x8ec07c);
#endif
  if (enumerator_->has_next()) {
    return enumerator_->next();
  }
  enumerator_->reset();

  evaluate();
  enumerate();

  if (enumerator_->has_next())
    return enumerator_->next();

  return nullptr;
}

void StreamAlgorithm::evaluate_start_char() {
  current_char = START_CHAR;
  evaluate_single_character();
  swap_state_lists();
}

void StreamAlgorithm::evaluate() {
  // read from buffer
  while (pos_i_ < end_i_) {
    stream_->read_back(current_char, pos_i_);
    evaluate_single_character();
    swap_state_lists();
    pos_i_++;

    if (!reached_final_states_.empty())
      return;
  }

  // read from stream
  if (segment_result == DoesNotFitInBuffer) {
    while (stream_->read(current_char)) {
      evaluate_single_character();
      swap_state_lists();
      pos_i_++;

      if (!reached_final_states_.empty())
        return;
    }
  }
}

void StreamAlgorithm::evaluate_single_character() {
  for (auto& current_state : current_states_) {

    std::vector<CaptureSubsetPair>* capture_subset_pairs =
        extended_det_va_.get_next_states(current_state, current_char);

    if (!capture_subset_pairs->empty()) {
      update_sets(current_state, *capture_subset_pairs);
    } else {
      ECS_interface_->unpin_node(current_state->output_node);
    }
  }
}

void StreamAlgorithm::update_sets(
    ExtendedDetVAState*& current_state,
    std::vector<CaptureSubsetPair> capture_subset_pairs) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif
  auto it = capture_subset_pairs.begin();

  // handle the empty capture
  if (capture_subset_pairs[0].capture.none()) {
    ExtendedDetVAState* next_state = capture_subset_pairs[0].subset;

    ECSNode* next_node = current_state->get_node();
    update_output_nodes(next_state, next_node);

    it++;
  }

  // handle not empty captures, skip first pair if already updated
  for (; it != capture_subset_pairs.end(); it++) {
    auto pair = *it;
    ExtendedDetVAState* next_state = pair.subset;
    std::bitset<64> capture = pair.capture;

    ECSNode* next_node = ECS_interface_->create_extend_node(
        current_state->get_node(), capture, pos_i_);
    update_output_nodes(next_state, next_node);
  }

  ECS_interface_->unpin_node(current_state->get_node());
}

void StreamAlgorithm::update_output_nodes(ExtendedDetVAState*& next_state,
                                          ECSNode*& next_node) {
  if (next_state->phase < (int64_t)pos_i_) {
    next_state->set_phase(pos_i_);

    next_state->set_node(next_node);
    ECS_interface_->pin_node(next_node);

    if (next_state->is_accepting()) {
      reached_final_states_.push_back(next_state);
    } else {
      next_states_.push_back(next_state);
    }

  } else {
    ECSNode* current_next_node = next_state->get_node();
    ECSNode* union_node =
        ECS_interface_->create_union_node(current_next_node, next_node);

    ECS_interface_->unpin_node(current_next_node);
    next_state->set_node(union_node);
    ECS_interface_->pin_node(union_node);
  }
}

void StreamAlgorithm::enumerate() {
  create_root_node_to_enumerate();

  if (ECS_root_node_ != nullptr)
    enumerator_->add_node(ECS_root_node_);
}

ECSNode* StreamAlgorithm::create_root_node_to_enumerate() {
  if (ECS_root_node_ != nullptr) {
    ECS_interface_->unpin_node(ECS_root_node_);
    ECS_root_node_ = nullptr;
  }

  for (auto& state : reached_final_states_) {
    if (ECS_root_node_ == nullptr) {
      // no need to pin/unpin the node here, it reuses the reference of the final state
      ECS_root_node_ = state->get_node();
    } else {
      ECSNode* union_node =
          ECS_interface_->create_union_node(ECS_root_node_, state->get_node());
      ECS_interface_->pin_node(union_node);
      ECS_interface_->unpin_node(ECS_root_node_);
      ECS_interface_->unpin_node(state->get_node());
      ECS_root_node_ = union_node;
    }
  }
  reached_final_states_.clear();
  return ECS_root_node_;
}

void StreamAlgorithm::swap_state_lists() {
  current_states_.swap(next_states_);
  next_states_.clear();
}

}  // namespace REmatch
