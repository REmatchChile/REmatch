#ifndef LIBRARY_INTERFACE__REGEX_HPP
#define LIBRARY_INTERFACE__REGEX_HPP

#include <vector>
#include <string>

#include "library_interface/match.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/match_iterator.hpp"

namespace rematch {

inline namespace library_interface {
  class Regex {
  public:
    Regex();
    ~Regex();

    Match find(std::string text, Flags flags = Flags());
    MatchIterator finditer(std::string text, Flags flags = Flags());
  };
} // end namespace library_interface

} // end namespace rematch

#endif
