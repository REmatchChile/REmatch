#include <emscripten/bind.h>

#include "library_interface/flags.hpp"
#include "library_interface/match.hpp"
#include "library_interface/match_iterator.hpp"
#include "library_interface/regex.hpp"
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

EMSCRIPTEN_BINDINGS(REmatch) {
  class_<RegexEmscriptenWrapper>("Regex")
      .constructor<std::string>()
      .function("finditer", &RegexEmscriptenWrapper::finditer_wrapper);

  class_<MatchIterator>("MatchIterator")
      .function("next", &MatchIterator::next)
      .function("variables", &MatchIterator::variables);

  class_<Match>("Match")
      .function("variables", &Match::variables)
      .function("start", select_overload<int(std::string)>(&Match::start))
      .function("end", select_overload<int(std::string)>(&Match::end));

  register_vector<std::string>("VectorString");
};
