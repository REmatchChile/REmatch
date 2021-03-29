#ifndef MATCH_ITERATOR_HPP
#define MATCH_ITERATOR_HPP

#include <iterator>

#include "match.hpp"
// #include "eval.hpp"
// #include "enumerator"

namespace rematch {

class Evaluator;

class MatchIterator {
 public:
  MatchIterator() = default;
  MatchIterator(Evaluator* e);

  Match_ptr next();

 private:
  Evaluator* evaluator_;
}; // end class MatchIterator

} // end namespace rematch

#endif // MATCH_ITERATOR_HPP