#include "udfa.hpp"

#include "automata/nfa/eva.hpp"

namespace rematch {

UDFA::UDFA(ExtendedVA const &A, size_t sz_thres)
    : nfa_size_(A.size()),
      ffactory_(A.filterFactory()),
      size_threshold_(sz_thres) {
  states_.reserve(A.states.size());

  initial_state_ = obtain_state(A.init_state());

  for(auto& capture: A.init_state()->captures) {
    State* nnq = obtain_state(capture->next);
    init_eval_states_.push_back({nnq, capture->code});
  }

  if(!A.init_state()->filters.empty()){
    init_eval_states_.push_back({initial_state_, 0});
  }
}

std::vector<UDFA::State*> UDFA::obtain_states(StateSubset const &ss) {
  // First search if the subset is found in cache

  std::vector<State*> ret;

  auto found = dstate_map_.find(ss.bitset);

  if(found != dstate_map_.end()) {
    ret.push_back(found->second);
  } else if (size() <= size_threshold_ || ss.subset.size() == 1) {
    State* nq = new State(ss.subset);
    states_.push_back(nq);
    dstate_map_.emplace_hint(found, ss.bitset, nq);
    ret.push_back(nq);
  } else {
    std::vector<bool> nss(nfa_size_, false);
    for (LogicalVA::State* q: ss.subset) {
      nss[q->id] = true;
      auto looked = dstate_map_.find(nss);
      if( looked != dstate_map_.end()) {
        ret.push_back(looked->second);
      } else {
        State* nq = new State(q);
        states_.push_back(nq);
        dstate_map_.emplace_hint(found, nss, nq);
        ret.push_back(nq);
      }
      nss[q->id] = false;
    }
  }

  return ret;
}

UDFA::State* UDFA::obtain_state(LogicalVA::State *q) {
  StateSubset ss(nfa_size_);
  ss.add(q);

  auto ret = obtain_states(ss);

  return ret[0];
}



Transition UDFA::next_transition(abstract::DState* dq, char a) {
  State* q = dynamic_cast<State*>(dq);
  std::vector<bool> chbst = ffactory_->apply_filters(a);

  auto ss = StateSubset(nfa_size_);

  auto &ntrans = q->transitions_[a];

  ntrans = Transition();

  for (LogicalVA::State* q_old: q->states_subset_) {
    for(auto& filter: q_old->filters) {
      if(chbst[filter->code]) ss.add(filter->next);
    }
  }

  for(State* nq: obtain_states(ss))
    ntrans->add_direct(nq);

  std::unordered_map<std::bitset<32>, StateSubset> reached_captures;

  for (LogicalVA::State* q_old: ss.subset) {
    for (auto &capture: q_old->captures) {
      auto it = reached_captures.find(capture->code);

      if(it == reached_captures.end()) {
        it = reached_captures.emplace(std::piecewise_construct,
                                      std::forward_as_tuple(capture->code),
                                      std::forward_as_tuple(nfa_size_)).first;
      }
      it->second.add(capture->next);
    }
  }

  for (auto &elem: reached_captures) {
    for(State* nq: obtain_states(elem.second))
      ntrans->add_capture({elem.first, nq});
  }

  return *ntrans;
}

} // end namespace rematch