#include <emscripten/bind.h>

#include "library_interface/flags.hpp"
#include "library_interface/match.hpp"
#include "library_interface/match_iterator.hpp"
#include "library_interface/query.hpp"
#include "library_interface/multi_query.hpp"
#include "library_interface/rematch.hpp"
#include "output_enumeration/span.hpp"

using namespace emscripten;
using namespace REMatch;


EMSCRIPTEN_BINDINGS(REmatch) {
  value_array<Span>("Span")
      .element(&Span::first)
      .element(&Span::second);

  class_<Query>("Regex")
      .constructor<std::string>()
      .function("finditer", &QueryEmscriptenWrapper::finditer_wrapper);

  class_<MatchIterator>("MatchIterator")
      .function("next", &MatchIterator::next)
      .function("variables", &MatchIterator::variables);

  class_<Match>("Match")
      .function("span", select_overload<Span(std::string)>(&Match::span));

  class_<MultiQuery>("MultiRegex")
      .constructor<std::string>()
      .function("finditer", &MultiQueryEmscriptenWrapper::finditer_wrapper);

  class_<MultiMatchIterator>("MultiMatchIterator")
      .function("next", &MultiMatchIterator::next)
      .function("variables", &MultiMatchIterator::variables);

  class_<MultiMatch>("MultiMatch")
      .function("spans", select_overload<std::vector<Span>(std::string)>(&MultiMatch::spans))
      .function("submatch", &MultiMatch::submatch);

  register_vector<std::string>("VectorString");
  register_vector<Span>("VectorSpan");

};
