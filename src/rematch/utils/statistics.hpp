
#pragma once

#include "mediator/mediator/mediator.hpp"

namespace REmatch {

struct Statistics {
  size_t extended_va_size = 0;
  size_t extended_det_va_size = 0;
  size_t search_nfa_size = 0;
  size_t search_dfa_size = 0;
  size_t nodes_allocated = 0;
  size_t nodes_used = 0;
  size_t nodes_reused = 0;
};

inline std::unique_ptr<Statistics> collect_statistics(
    const Mediator& mediator) {
  auto stats = std::make_unique<Statistics>();
  stats->extended_va_size = mediator.algorithm_->get_extended_va_size();
  stats->extended_det_va_size = mediator.algorithm_->get_extended_det_va_size();
  stats->search_nfa_size = mediator.segment_manager_->get_search_nfa_size();
  stats->search_dfa_size = mediator.segment_manager_->get_search_dfa_size();
  stats->nodes_allocated = mediator.algorithm_->get_amount_of_nodes_used();
  stats->nodes_used = mediator.algorithm_->get_amount_of_nodes_used();
  stats->nodes_reused = mediator.algorithm_->get_amount_of_nodes_reused();

  return stats;
}

}  // namespace REmatch
