#include "matchiterator.hpp"

#include "evaluation/evaluator.hpp"
#include "match.hpp"

namespace rematch {

MatchIterator::MatchIterator(Evaluator *e, EvalStats *s)
    : evaluator_(e), stats_(s) {}

Match_ptr MatchIterator::next() { return evaluator_->next(); }

} // end namespace rematch