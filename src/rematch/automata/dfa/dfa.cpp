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
  StateSubset ss(eVA_.size());
  ss.add(eVA_.init_state());

  init_state_ = obtain_state(ss);

  // Compute init eval states
  std::unordered_map<std::bitset<32>, StateSubset> reach_captures;

  for (auto &capture : eVA_.init_state()->captures) {
    auto res = reach_captures.emplace(capture->code, StateSubset(eVA_.size()));
    res.first->second.add(capture->next);
  }

  for (auto &elem : reach_captures) {
    auto nq = obtain_state(elem.second);
    init_eval_states_.emplace_back(std::make_pair(nq, elem.first));
  }

  if (!eVA_.init_state()->filters.empty()) {
    init_eval_states_.emplace_back(std::make_pair(init_state_, 0));
  }
}

DFA::State* DFA::obtain_state(StateSubset ss) {
  auto found = dstates_table_.find(ss.bitset);

  State *nq;

  if (found == dstates_table_.end()) { // Check if already stored subset
    nq = new State(ss.subset);

    found = dstates_table_.emplace_hint(found, ss.bitset, nq);

    states.push_back(nq);

    if (nq->accepting()) {
      finalStates.push_back(nq);
    }
  }

  return found->second;
}



size_t DFA::tot_size() const {
  size_t res = 0;
  for(auto &p: states) {
    res += p->states_subset_.size() * sizeof(LogicalVA::State*);
    res += sizeof(State);
  }

  for(auto &t: transitions) {
    res += t->captures_.size() * sizeof(Transition::Capture);
    res += t->directs_.size() * sizeof(abstract::DState*);
    res += sizeof(Transition);
  };

  return sizeof(*this) + res;
}

#ifdef NOPT_ASCIIARRAY

Transition DFA::next_base_transition(abstract::DState *dq, char a) {
  auto *q = dynamic_cast<State*>(dq);

  std::vector<bool> char_bitset = ffactory_->apply_filters(a);

  auto found = q->base_transitions_.find(char_bitset);

  if(found != q->base_transitions_.end()) return found->second;

  Transition ntrans = compute_transition(q, char_bitset);

  q->base_transitions_.emplace_hint(found, char_bitset, ntrans);

  return ntrans;
  
}

#else

Transition DFA::next_transition(abstract::DState *dq, char a) {

  State* q = dynamic_cast<State*>(dq);

  auto &ntrans = q->transitions_[a];

  if(ntrans) return *ntrans;

  std::vector<bool> chbst = ffactory_->apply_filters(a);

  ntrans = compute_transition(q, chbst);

  return *ntrans;
}

#endif

Transition DFA::compute_transition(State* q, std::vector<bool> chbst) {
  StateSubset ss(eVA_.size());

  Transition ntrans;

  for (auto &state : q->subset()) {
    for (auto &filter : state->filters) {
      if (chbst[filter->code]) ss.add(filter->next);
    }
  }

  State* nq = obtain_state(ss);

  if (ss.subset.empty()) return ntrans;

  ntrans.add({0,nq});

  std::unordered_map<std::bitset<32>, StateSubset> capture_reach;

  for (LogicalVA::State* q_old: ss.subset) {
    for (auto &capture: q_old->captures) {
      auto it = capture_reach.find(capture->code);

      if(it == capture_reach.end()) {
        it = capture_reach.emplace(std::piecewise_construct,
                                  std::forward_as_tuple(capture->code),
                                  std::forward_as_tuple(eVA_.size())).first;
      }
      it->second.add(capture->next);
    }
  }

  for (auto &elem: capture_reach) {
    auto S = elem.first;
    auto nq = obtain_state(elem.second);
    ntrans.add({S, nq});
  }

  return ntrans;
}

} // end namespace rematch
