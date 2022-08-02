#include "dfa.hpp"

#include <algorithm>
#include <cassert>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "automata/dfa/dstate.hpp"
#include "automata/dfa/transition.hpp"
#include "automata/nfa/eva.hpp"
#include "automata/nfa/state.hpp"
#include "factories/factories.hpp"
#include "structs/dag/nodelist.hpp"

namespace rematch {

DFA::DFA(ExtendedVA const &A)
    : init_state_(nullptr), eVA_(A), variable_factory_(A.varFactory()),
      ffactory_(A.filterFactory()) {

  // Code initial state
  std::set<LogicalVA::State*> new_subset;
  new_subset.insert(eVA_.init_state());

  init_state_ = new State(eVA_.size(), new_subset);

  dstates_table_[init_state_->bitmap()] = init_state_;

  // Compute init eval states

  std::unordered_map<std::bitset<32>, std::set<LogicalVA::State *>> reach_captures;

  for (auto &capture : eVA_.init_state()->captures) {
    auto res = reach_captures.emplace(capture->code, std::set<LogicalVA::State *>());
    res.first->second.insert(capture->next);
  }

  for (auto &elem : reach_captures) {
    State *nq = new State(eVA_.size(), elem.second);

    auto found = dstates_table_.find(nq->bitmap());

    if (found == dstates_table_.end()) {
      dstates_table_[nq->bitmap()] = nq;

      states.push_back(nq);

      if (nq->accepting())
        finalStates.push_back(nq);
    } else {
      delete nq;
      nq = found->second;
    }

    init_eval_states_.emplace_back(std::make_pair(nq, elem.first));
  }

  if (!eVA_.init_state()->filters.empty()) {
    init_eval_states_.emplace_back(std::make_pair(init_state_, 0));
  }
}

Transition<DFA::State> *DFA::next_transition(State *q, char a) {

  std::vector<bool> char_bitset = ffactory_->apply_filters(a);

  std::set<LogicalVA::State *> new_ss;                   // Store the next subset
  StatesBitmap ss_bitset(eVA_.size(), false); // Subset bitset representation

  for (auto &state : q->subset()) {

    for (auto &filter : state->filters) {
      if (char_bitset[filter->code] && !ss_bitset[filter->next->id]) {
        new_ss.insert(filter->next);
        ss_bitset[filter->next->id] = true;
      }
    }
  }

  auto found = dstates_table_.find(ss_bitset);

  State *nq;

  if (found == dstates_table_.end()) { // Check if already stored subset
    nq = new State(eVA_.size(), new_ss);

    found = dstates_table_.emplace_hint(found, std::make_pair(ss_bitset, nq));

    states.push_back(nq);

    if (nq->accepting()) {
      finalStates.push_back(nq);
    }
  }
  nq = found->second;

  if (!nq->empty_subset()) {
    compute_captures(q, nq, a);
    q->add_direct(a, nq);
  } else {
    q->add_empty(a);
  }

  return q->next_transition(a);
}

void DFA::compute_captures(State *p, State *q, char a) {

  std::unordered_map<std::bitset<32>,
                     std::pair<std::set<LogicalVA::State*>, StatesBitmap>>
      capture_reach;

  for (auto &extState : q->subset()) {
    for (auto &capture : extState->captures) {

      auto it = capture_reach.find(capture->code);

      if (it == capture_reach.end()) {
        it = capture_reach
                 .emplace(capture->code,
                          std::make_pair(std::set<LogicalVA::State *>(),
                                         StatesBitmap(eVA_.size(), false)))
                 .first;
      }

      it->second.first.insert(capture->next);
      it->second.second[capture->next->id] = true;
    }
  }

  for (auto el : capture_reach) {
    /* Check if subset is not on hash table */

    auto found = dstates_table_.find(el.second.second);

    if (found == dstates_table_.end()) {
      State *nq = new State(eVA_.size(), el.second.first);
      found =
          dstates_table_.emplace_hint(found, std::make_pair(nq->bitmap(), nq));

      states.push_back(nq);

      if (nq->accepting()) {
        finalStates.push_back(nq);
      }
    }

    // This is the deterministic state where the capture reaches
    State *r = found->second;

    p->add_capture(a, el.first, r);
  }
}

} // end namespace rematch
