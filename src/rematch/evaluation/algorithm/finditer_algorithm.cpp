#include "finditer_algorithm.hpp"

#include <cstdint>
#include <utility>

#include "evaluation/log.hpp"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace REmatch {

FinditerAlgorithm::FinditerAlgorithm(ExtendedVA& extended_va,
                                     uint_fast32_t max_mempool_duplications,
                                     uint_fast32_t max_deterministic_states) {
  ECS_interface_ = std::make_unique<ECS>(max_mempool_duplications);
  enumerator_ = std::make_unique<Enumerator>();
  extended_det_va_ = std::make_unique<ExtendedDetVA>(extended_va, max_deterministic_states);

  ExtendedDetVAState* initial_state = extended_det_va_->get_initial_state();
  ECSNode* bottom_node = ECS_interface_->create_bottom_node();
  initial_state->set_node(bottom_node);
  ECS_interface_->pin_node(bottom_node);
}

void FinditerAlgorithm::begin(std::shared_ptr<TextWrapper> document) {
  document_ = std::move(document);
  doc_end_i_ = static_cast<int64_t>(document_->size());
  reset();
}

void FinditerAlgorithm::reset() {
  pos_i_ = doc_start_i_;
  current_states_.clear();
  next_states_.clear();
  reached_final_states_.clear();

  extended_det_va_->set_state_initial_phases();
  ExtendedDetVAState* initial_state = extended_det_va_->get_initial_state();
  current_states_.push_back(initial_state);
}

void FinditerAlgorithm::set_document_indexes(Span& span) {
  doc_start_i_ = span.first;
  doc_end_i_ = span.second;
}

const Mapping* FinditerAlgorithm::next() {
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

void FinditerAlgorithm::evaluate_start_char() {
  current_char = START_CHAR;
  evaluate_single_character();
  swap_state_lists();
}

void FinditerAlgorithm::evaluate() {

  while (pos_i_ < doc_end_i_) {
    current_char = (*document_)[pos_i_];
    evaluate_single_character();
    swap_state_lists();
    pos_i_++;

    if (!reached_final_states_.empty())
      return;
  }
}

void FinditerAlgorithm::evaluate_single_character() {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif
  LOG("pos_i " << pos_i_ << ": " << current_char << '\n');

  for (auto& current_state : current_states_) {
    LOG("  current: " << current_state->id << '\n');

    auto* capture_subset_pairs = extended_det_va_->get_next_states(current_state, current_char);

    if (!capture_subset_pairs->empty()) {
      update_sets(current_state, *capture_subset_pairs);
    } else {
      ECS_interface_->unpin_node(current_state->output_node);
    }
  }
}

void FinditerAlgorithm::update_sets(ExtendedDetVAState* current_state,
                                    const std::vector<CaptureSubsetPair>& capture_subset_pairs) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif

  auto it = capture_subset_pairs.begin();

  // handle the empty capture
  if (it->capture.none()) {
    auto* next_state = capture_subset_pairs[0].subset;
    LOG("    reached: " << next_state->id << '\n');

    auto* next_node = current_state->get_node();
    update_output_nodes(next_state, next_node);
    ++it;
  }

  // handle the non-empty captures
  while (it != capture_subset_pairs.end()) {
    auto* next_node =
        ECS_interface_->create_extend_node(current_state->get_node(), it->capture, pos_i_);

    auto* next_state = it->subset;
    LOG("    reached: " << next_state->id << '\n');
    update_output_nodes(next_state, next_node);
    ++it;
  }

  ECS_interface_->unpin_node(current_state->get_node());
}

void FinditerAlgorithm::update_output_nodes(ExtendedDetVAState* next_state, ECSNode* next_node) {
#ifdef TRACY_ENABLE
  ZoneScoped;
#endif

  if (next_state->phase < pos_i_) {
    next_state->set_phase(pos_i_);

    next_state->set_node(next_node);
    ECS_interface_->pin_node(next_node);

    if (next_state->is_accepting()) {
      reached_final_states_.emplace_back(next_state);
    } else {
      next_states_.emplace_back(next_state);
    }

  } else {
    ECSNode* current_next_node = next_state->get_node();
    ECSNode* union_node = ECS_interface_->create_union_node(current_next_node, next_node);

    ECS_interface_->unpin_node(current_next_node);
    next_state->set_node(union_node);
    ECS_interface_->pin_node(union_node);
  }
}

void FinditerAlgorithm::enumerate() {
  create_root_node_to_enumerate();

  if (ECS_root_node_ != nullptr)
    enumerator_->add_node(ECS_root_node_);
}

ECSNode* FinditerAlgorithm::create_root_node_to_enumerate() {
  if (ECS_root_node_ != nullptr) {
    ECS_interface_->unpin_node(ECS_root_node_);
    ECS_root_node_ = nullptr;
  }

  for (auto& state : reached_final_states_) {
    if (ECS_root_node_ == nullptr) {
      // no need to pin/unpin the node here, it reuses the reference of the final state
      ECS_root_node_ = state->get_node();
    } else {
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

}  // namespace REmatch
