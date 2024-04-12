#include <emscripten/bind.h>

#include "library_interface/flags.hpp"
#include "library_interface/match.hpp"
#include "library_interface/match_iterator.hpp"
#include "library_interface/regex.hpp"
#include "library_interface/multi_regex.hpp"
#include "library_interface/rematch.hpp"
#include "output_enumeration/span.hpp"

using namespace emscripten;
using namespace REMatch;

/**
 * As Emscripten doesn't support std::string_view as argument, we have to bind
 * the to std::string by copy instead.
 */
class RegexEmscriptenWrapper : public Regex {
 public:
  explicit RegexEmscriptenWrapper(std::string pattern)
      : Regex(std::string_view(pattern)) {}

  std::unique_ptr<MatchIterator> finditer_wrapper(std::string str) {
    return finditer(std::string_view(str));
  }
};

class MultiRegexEmscriptenWrapper : public MultiRegex {
 public:
  explicit MultiRegexEmscriptenWrapper(std::string pattern)
      : MultiRegex(std::string_view(pattern)) {}

  std::unique_ptr<MultiMatchIterator> finditer_wrapper(std::string str) {
    return finditer(std::string_view(str));
  }
};

EMSCRIPTEN_BINDINGS(REmatch) {
  value_array<Span>("Span")
      .element(&Span::first)
      .element(&Span::second);

  class_<RegexEmscriptenWrapper>("Regex")
      .constructor<std::string>()
      .function("finditer", &RegexEmscriptenWrapper::finditer_wrapper);

  class_<MatchIterator>("MatchIterator")
      .function("next", &MatchIterator::next)
      .function("variables", &MatchIterator::variables);

  class_<Match>("Match")
      .function("span", select_overload<Span(std::string)>(&Match::span));

  class_<MultiRegexEmscriptenWrapper>("MultiRegex")
      .constructor<std::string>()
      .function("finditer", &MultiRegexEmscriptenWrapper::finditer_wrapper);

  class_<MultiMatchIterator>("MultiMatchIterator")
      .function("next", &MultiMatchIterator::next)
      .function("variables", &MultiMatchIterator::variables);

  class_<MultiMatch>("MultiMatch")
      .function("spans", select_overload<std::vector<Span>(std::string)>(&MultiMatch::spans))
      .function("submatch", &MultiMatch::submatch);

  register_vector<std::string>("VectorString");
  register_vector<Span>("VectorSpan");

};
