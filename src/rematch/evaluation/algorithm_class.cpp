#include "algorithm_class.hpp"

namespace rematch {

AlgorithmClass::AlgorithmClass(ExtendedDetVA &extended_det_va,
                               std::string_view document, Flags flags)
    : extended_det_va_(extended_det_va) {
  document_ = document;
  ECS_interface_ = new ECS(flags);
  enumerator_ = new Enumerator();

  ExtendedDetVAState* initial_state = extended_det_va_.get_initial_state();
  ECSNode* bottom_node = ECS_interface_->create_bottom_node();
  initial_state->set_node(bottom_node);
  ECS_interface_->pin_node(bottom_node);

  initialize_algorithm();
}

void AlgorithmClass::initialize_algorithm() {
  pos_i_ = 0;
  current_states_.clear();
  next_states_.clear();
  reached_final_states_.clear();

  extended_det_va_.set_state_initial_phases();
  ExtendedDetVAState* initial_state = extended_det_va_.get_initial_state();
  current_states_.push_back(initial_state);
}

void AlgorithmClass::set_ecs(ECS& ecs) {
  ECS_interface_ = &ecs;
}

void AlgorithmClass::set_document(std::string document) {
  document_ = document;
}

const Mapping* AlgorithmClass::get_next_mapping() {
  ZoneScopedC(0x808000);

  if (enumerator_->has_next()) {
    return enumerator_->next();
  } else {
    enumerator_->reset();
  }

  evaluate();
  enumerate();

  if (enumerator_->has_next())
    return enumerator_->next();

  return nullptr;
}

void AlgorithmClass::evaluate() {

  while (pos_i_ < document_.size()) {
    evaluate_single_character();
    swap_state_lists();
    pos_i_++;

    if (!reached_final_states_.empty())
      return;
  }
}

void AlgorithmClass::evaluate_single_character() {
  char letter = document_[pos_i_];

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

void AlgorithmClass::update_output_nodes(ExtendedDetVAState*& next_state,
                                         ECSNode*& next_node) {

  if (next_state->phase < (int64_t) pos_i_) {
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
    ECSNode* union_node = ECS_interface_->create_union_node(current_next_node, next_node);

    ECS_interface_->unpin_node(current_next_node);
    next_state->set_node(union_node);
    ECS_interface_->pin_node(union_node);
  }
}

void AlgorithmClass::enumerate() {
  create_root_node_to_enumerate();

  if (ECS_root_node_ != nullptr)
    enumerator_->add_node(ECS_root_node_);
}

ECSNode* AlgorithmClass::create_root_node_to_enumerate() {
  if (ECS_root_node_ != nullptr) {
    ECS_interface_->unpin_node(ECS_root_node_);
    ECS_root_node_ = nullptr;
  }

  for (auto& state : reached_final_states_) {
    if (ECS_root_node_ == nullptr){
      // no need to pin/unpin the node here, it reuses the reference of the final state
      ECS_root_node_ = state->get_node();
    }
    else {
      ECSNode* union_node = ECS_interface_->create_union_node(ECS_root_node_, state->get_node());
      ECS_interface_->pin_node(union_node);
      ECS_interface_->unpin_node(ECS_root_node_);
      ECS_interface_->unpin_node(state->get_node());
      ECS_root_node_ = union_node;
    }
  }
  reached_final_states_.clear();
  return ECS_root_node_;
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

}
