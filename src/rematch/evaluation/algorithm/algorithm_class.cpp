#include "algorithm_class.hpp"

#include "evaluation/document.hpp"

using namespace REmatch;

AlgorithmClass::AlgorithmClass(ExtendedVA& extended_va,
                               std::shared_ptr<Document> document,
                               uint_fast32_t max_mempool_duplications, uint_fast32_t max_deterministic_states)
    : doc_end_i_(document->size()),
      document_(document),
      extended_det_va_(extended_va, max_deterministic_states) {
  ECS_interface_ = std::make_unique<ECS>(max_mempool_duplications);
  enumerator_ = std::make_unique<Enumerator>();

  ExtendedDetVAState* initial_state = extended_det_va_.get_initial_state();
  ECSNode* bottom_node = ECS_interface_->create_bottom_node();
  initial_state->set_node(bottom_node);
  ECS_interface_->pin_node(bottom_node);

  initialize_algorithm();
}

void AlgorithmClass::initialize_algorithm() {
  pos_i_ = doc_start_i_;
  current_states_.clear();
  next_states_.clear();
  reached_final_states_.clear();

  extended_det_va_.set_state_initial_phases();
  ExtendedDetVAState* initial_state = extended_det_va_.get_initial_state();
  current_states_.push_back(initial_state);
}

ECS& AlgorithmClass::get_ecs() {
  return *ECS_interface_;
}

void AlgorithmClass::set_document_indexes(Span& span) {
  doc_start_i_ = span.first;
  doc_end_i_ = span.second;
}

void AlgorithmClass::set_null_segment() {
  doc_end_i_ = doc_start_i_;
}

void AlgorithmClass::evaluate_single_character() {
  char letter = (*document_)[pos_i_];

  for (auto& current_state : current_states_) {

    std::vector<CaptureSubsetPair> capture_subset_pairs =
        extended_det_va_.get_next_states(current_state, letter);

    if (!capture_subset_pairs.empty()) {
      update_sets(current_state, capture_subset_pairs);
    }
    else {
      ECS_interface_->unpin_node(current_state->output_node);
    }
  }
}

void AlgorithmClass::update_sets(
    ExtendedDetVAState*& current_state,
    std::vector<CaptureSubsetPair> capture_subset_pairs) {

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

void AlgorithmClass::swap_state_lists() {
  current_states_.swap(next_states_);
  next_states_.clear();
}

size_t AlgorithmClass::get_extended_det_va_size() {
  return extended_det_va_.states.size();
}

size_t AlgorithmClass::get_extended_va_size() {
  return extended_det_va_.get_extended_va_size();
}

size_t AlgorithmClass::get_amount_of_nodes_allocated() {
  return ECS_interface_->amount_of_nodes_allocated();
}

size_t AlgorithmClass::get_amount_of_nodes_reused() {
  return ECS_interface_->get_amount_of_nodes_reused();
}

size_t AlgorithmClass::get_amount_of_nodes_used() {
  return ECS_interface_->get_amount_of_nodes_used();
}

