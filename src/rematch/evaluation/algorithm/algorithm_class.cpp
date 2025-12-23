#include "algorithm_class.hpp"

#include "evaluation/document.hpp"

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

using namespace REmatch;

ECS& AlgorithmClass::get_ecs() const {
  return *ECS_interface_;
}

void AlgorithmClass::swap_state_lists() {
  current_states_.swap(next_states_);
  next_states_.clear();
}

size_t AlgorithmClass::get_extended_det_va_size() const {
  return extended_det_va_->states.size();
}

size_t AlgorithmClass::get_extended_va_size() const {
  return extended_det_va_->get_extended_va_size();
}

size_t AlgorithmClass::get_amount_of_nodes_allocated() const {
  return ECS_interface_->amount_of_nodes_allocated();
}

size_t AlgorithmClass::get_amount_of_nodes_reused() const {
  return ECS_interface_->get_amount_of_nodes_reused();
}

size_t AlgorithmClass::get_amount_of_nodes_used() const {
  return ECS_interface_->get_amount_of_nodes_used();
}
