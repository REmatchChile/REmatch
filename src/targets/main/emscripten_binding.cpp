#include <emscripten/bind.h>

#include "library_interface/flags.hpp"
#include "library_interface/match.hpp"
#include "library_interface/match_iterator.hpp"
#include "library_interface/regex.hpp"
#include "library_interface/rematch.hpp"
#include "output_enumeration/span.hpp"

using namespace emscripten;
using namespace REMatch;

EMSCRIPTEN_BINDINGS(REmatch) {
  class_<Flags>("Flags")
    .constructor<>();
  class_<MatchIterator>("MatchIterator")
    .function("next", &MatchIterator::next);
  class_<Match>("Match")
    .function("variables", &Match::variables)
    .function("start", select_overload<int(std::string)>(&Match::start))
    .function("end", select_overload<int(std::string)>(&Match::end));
  class_<Regex>("Regex")
    .function("finditer", select_overload<MatchIterator(const std::string&, Flags)>(&Regex::finditer));

  register_vector<std::string>("VectorString");

  function("compile", select_overload<Regex(const std::string&, Flags)>(&compile));
};
