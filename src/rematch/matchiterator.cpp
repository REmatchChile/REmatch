#include "matchiterator.hpp"

#include "evaluation/evaluator.hpp"
#include "match.hpp"

namespace rematch {

MatchIterator::MatchIterator(Evaluator* e)
    : evaluator_(e) {}

Match_ptr MatchIterator::next() {
  return evaluator_->next();
}

} // end namespace rematch