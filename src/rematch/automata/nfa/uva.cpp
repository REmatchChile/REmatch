#include "uva.hpp"

#include "automata/nfa/state.hpp"
#include "factories/factories.hpp"

namespace rematch {

UnambiguousVA::UnambiguousVA(ExtendedVA const &A) : ffactory_(A.filterFactory()) {
  states_.reserve(A.states.size());
  initial_state_ = obtain_ustate(A.init_state());
  accepting_state_ = obtain_ustate(A.accepting_state());

  for(auto& capture: A.init_state()->captures) {
    State* nnq = obtain_ustate(capture->next);
    init_eval_states_.push_back({nnq, capture->code});
  }

  if(!A.init_state()->filters.empty()){
    init_eval_states_.push_back({initial_state_, 0});
  }
}

UnambiguousVA::State* UnambiguousVA::obtain_ustate(LogicalVA::State* p) {
  auto found = state_bimap_.right.find(p);
  State* nq;
  if(found == state_bimap_.right.end()) {
    nq = &states_.emplace_back(*p);
    state_bimap_.insert({nq, p});
  } else {
    nq = found->second;
  }
  return nq;
}

Transition<UnambiguousVA::State>* UnambiguousVA::next_transition(State* q, char a) {
  const LogicalVA::State* q_old = state_bimap_.left.at(q);
  std::vector<bool> char_bitset = ffactory_->apply_filters(a);

  auto *ntrans = new Transition<State>();

  q->transitions[a] = ntrans; // defaults to empty transition

  for(auto& filter: q_old->filters) {
    if(char_bitset[filter->code]) {
      State* nq = obtain_ustate(filter->next);
      ntrans->add_direct(nq);

      for(auto& capture: filter->next->captures) {
        State* nnq = obtain_ustate(capture->next);
        ntrans->add_capture({capture->code, nnq});
      }
    }
  }

  return q->transitions[a];
}

} // end namespace rematch