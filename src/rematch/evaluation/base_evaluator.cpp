#include "base_evaluator.hpp"

namespace rematch {

BaseEvaluator::BaseEvaluator(RegEx &rgx, std::shared_ptr<StrDocument> d,
                             Anchor a, EvalStats &e)
    : rgx_(rgx), enumerator_(rgx_), text_(d), stats_(e) {

  eva_ = rgx_.extendedVA();
  dfa_ = std::make_unique<DFA>(*eva_);

  stats_.eva_size = eva_->size();

  T_[dfa_->init_state()] = ds_.bottom_node();
}

Match_ptr BaseEvaluator::next() {

 Enumeration:
  if (enumerator_.has_next())
    return enumerator_.next();

  for(; i_pos_ < text_->size(); i_pos_++) {
    char a = (*text_)[i_pos_] & 0x7F;

    T_prim_.clear();

    for(auto &elem: T_) {
      auto* q = elem.first;

      auto ntrans = dfa_->next_base_transition(q, a);

      for(auto &capture: ntrans.captures) {
        DFA::State *qnext = dynamic_cast<DFA::State*>(capture.next);

        ECS::Node *oid = T_[q];

        if(capture.S != 0)
          oid = ds_.extend(oid, capture.S, i_pos_);
        if(T_prim_.contains(qnext))
          oid = ds_.unite(oid, T_prim_[qnext]);
        T_prim_[qnext] = oid;
      }
    }
    T_.swap(T_prim_);
  }

  if(i_pos_ == text_->size() && text_->size() > 0) {
    for (auto &elem : T_) {
      auto *state = elem.first;
      if(state->accepting())
        enumerator_.add_node(elem.second);
    }
    i_pos_++;
    goto Enumeration;
  }

  return nullptr;
}

} // end namespace rematch