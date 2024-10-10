#include "finditer_algorithm.hpp"

namespace REmatch {

FinditerAlgorithm::FinditerAlgorithm(ExtendedVA& extended_va,
                                     std::shared_ptr<Document> document,
                                     uint_fast32_t max_mempool_duplications,
                                     uint_fast32_t max_deterministic_states)
    : AlgorithmClass(extended_va, document, max_mempool_duplications,
                     max_deterministic_states) {}

const Mapping* FinditerAlgorithm::get_next_mapping() {
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

void FinditerAlgorithm::evaluate() {

  while (pos_i_ < doc_end_i_) {
    evaluate_single_character();
    swap_state_lists();
    pos_i_++;

    if (!reached_final_states_.empty())
      return;
  }
}

void FinditerAlgorithm::update_output_nodes(ExtendedDetVAState*& next_state,
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

}  // namespace REmatch
