#include "findone_algorithm.hpp"
#include <cstdint>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace REmatch {

FindoneAlgorithm::FindoneAlgorithm(ExtendedVA& extended_va,
                                   std::shared_ptr<Document> document,
                                   uint_fast32_t max_mempool_duplications,
                                   uint_fast32_t max_deterministic_states)
    : AlgorithmClass(extended_va, document, max_mempool_duplications,
                     max_deterministic_states) {}

const Mapping* FindoneAlgorithm::get_next_mapping() {
  evaluate();
  enumerate();

  if (enumerator_->has_next())
    return enumerator_->next();

  return nullptr;
}

void FindoneAlgorithm::evaluate() {
  while (pos_i_ < doc_end_i_) {
    evaluate_single_character();
    swap_state_lists();
    pos_i_++;

    if (!reached_final_states_.empty())
      return;
  }
}

void FindoneAlgorithm::update_output_nodes(ExtendedDetVAState* next_state,
                                           ECSNode* next_node) {
  #ifdef TRACY_ENABLE
  ZoneScoped;
  #endif

  const auto pos_i_int_ = static_cast<int>(pos_i_);
  if (next_state->phase < pos_i_int_) {
    next_state->set_phase(pos_i_int_);

    next_state->set_node(next_node);
    ECS_interface_->pin_node(next_node);

    if (next_state->is_accepting()) {
      reached_final_states_.emplace_back(next_state);
    } else {
      next_states_.emplace_back(next_state);
    }
  } else {
    // the node manager needs to unpin the node in order to free it
    ECS_interface_->pin_node(next_node);
    ECS_interface_->unpin_node(next_node);
  }
}

void FindoneAlgorithm::enumerate() {
  if (!reached_final_states_.empty()) {
    ExtendedDetVAState* final_state = reached_final_states_.front();
    enumerator_->add_node(final_state->get_node());
  }
}

}  // namespace REmatch
