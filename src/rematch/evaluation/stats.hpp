#ifndef EVALUATION_EVALUATION_STATS_HPP
#define EVALUATION_EVALUATION_STATS_HPP

#include <vector>

namespace rematch {

struct EvalStats {

  EvalStats()
    : n_search_intervals(0),
      eva_size(0),
      dfa_size(0),
      sva_size(0),
      sdfa_size(0) {}

  size_t n_search_intervals;
  std::vector<std::pair<int, int>> search_intervals;
  size_t eva_size;
  size_t dfa_size;
  size_t sva_size;
  size_t sdfa_size;

}; // end class EvalStats

} // end namespace rematch

#endif // EVALUATION_EVALUATION_STATS_HPP
