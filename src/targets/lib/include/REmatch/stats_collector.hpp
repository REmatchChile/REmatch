#pragma once

#include "mediator/mediator/mediator.hpp"
#include "statistics.hpp"

namespace REmatch {

class StatsCollector {
 public:
  Statistics collect(Mediator* mediator) {
    Statistics stats;
    stats.extended_va_size = mediator->algorithm_->get_extended_va_size();
    stats.extended_det_va_size =
        mediator->algorithm_->get_extended_det_va_size();
    stats.search_nfa_size = mediator->segment_manager_->get_search_nfa_size();
    stats.search_dfa_size = mediator->segment_manager_->get_search_dfa_size();
    stats.nodes_allocated = mediator->algorithm_->get_amount_of_nodes_used();
    stats.nodes_used = mediator->algorithm_->get_amount_of_nodes_used();
    stats.nodes_reused = mediator->algorithm_->get_amount_of_nodes_reused();

    return stats;
  }
};

}  // namespace REmatchF
