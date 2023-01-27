#ifndef LIBRARY_INTERFACE__REGEX_HPP
#define LIBRARY_INTERFACE__REGEX_HPP

#include <vector>
#include <string>
#include <optional>

#include "library_interface/match.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/match_iterator.hpp"

namespace REMatch {
inline namespace library_interface {

class Regex {
private:
  rematch::SearchDFA *search_dfa;

public:
  Regex(std::string_view pattern, Flags flags = Flags());
  ~Regex();

  std::optional<Match> find(std::string_view text, Flags flags = Flags());
  MatchIterator finditer(std::string_view text, Flags flags = Flags());
};

} // end namespace library_interface
} // end namespace rematch

#endif
