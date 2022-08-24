#ifndef EVALUATION_EVALUATION_STATS_HPP
#define EVALUATION_EVALUATION_STATS_HPP

#include <vector>

namespace rematch {

struct EvalStats {
  size_t n_search_intervals = 0;
  std::vector<std::pair<int, int>> search_intervals;
  size_t eva_size = 0;
  size_t dfa_size = 0;
  size_t sva_size = 0;
  size_t sdfa_size = 0;
  size_t mdfa_size = 0;
  size_t n_nodes = 0;
  size_t n_reused_nodes = 0;
  size_t pool_total_size = 0;
  size_t dfa_total_size = 0;
}; // end class EvalStats

} // end namespace rematch

#endif // EVALUATION_EVALUATION_STATS_HPP
