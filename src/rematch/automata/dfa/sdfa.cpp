#include "sdfa.hpp"

#include <set>
#include <string>
#include <vector>

#include "factories/factories.hpp"

namespace rematch {

SearchDFA::SearchDFA(SearchVA const &A)
    : has_epsilon_(A.has_epsilon()), sVA_(A), vfactory_(A.variable_factory()),
      ffactory_(A.filter_factory()) {
  initial_state_ = new_dstate();
  initial_state_->add_state(A.initial_state());

  initial_state_->set_initial(true);

  dstates_table_[initial_state_->bitmap()] = initial_state_;
}

SDState *SearchDFA::new_dstate() {
  SDState *np = new SDState(sVA_.size());
  states.push_back(np);
  return np;
}

SDState *SearchDFA::next_state(SDState *q, char a) {

  std::vector<bool> triggered_filters = ffactory_->applyFilters(a);

  std::set<State *> new_subset;              // Store the next subset
  std::vector<bool> new_bitset(sVA_.size()); // Subset bitset representation

  for (auto &state : q->subset()) {
    for (auto &filter : state->filters) {
      if (triggered_filters[filter->code] && !new_bitset[filter->next->id]) {
        new_subset.insert(filter->next);
        new_bitset[filter->next->id] = true;
      }
    }
  }

  // Add a bit to the end of the hashable key
  if (new_subset.empty())
    new_bitset.push_back(true);
  else
    new_bitset.push_back(false);

  // Add the states coming from an init state
  for (auto &state : initial_state_->subset()) {
    for (auto &filter : state->filters) {
      if (triggered_filters[filter->code] && !new_bitset[filter->next->id]) {
        new_subset.insert(filter->next);
        new_bitset[filter->next->id] = true;
      }
    }
  }

  auto found = dstates_table_.find(new_bitset);

  if (found == dstates_table_.end()) { // Check if already stored subset
    auto nq = new SDState(sVA_.size(), new_subset);

    found = dstates_table_.emplace_hint(found, std::make_pair(new_bitset, nq));

    states.push_back(nq);

    if (nq->accepting())
      final_states.push_back(nq);

    // Store if the runs end here
    nq->set_ends(new_bitset.back());
  }

  q->set_transition(a, found->second);

  return found->second;
}

} // end namespace rematch
