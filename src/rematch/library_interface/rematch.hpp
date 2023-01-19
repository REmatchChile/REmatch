#ifndef LIBRARY_INTERFACE__REMATCH_HPP
#define LIBRARY_INTERFACE__REMATCH_HPP

#include <vector>
#include <string>

#include "library_interface/match.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/match_iterator.hpp"
#include "library_interface/regex.hpp"

namespace rematch {
// TODO: Maybe differentiate between the internal code and the library interface
// using different namespaces?

inline namespace library_interface {
  class REMatch {
  public:
    REMatch();
    ~REMatch();

    Regex compile(std::string pattern, Flags& flags);

    Match find(std::string pattern, std::string text, Flags flags = Flags());
    std::vector<Match> findall(std::string pattern, std::string text,
                            Flags flags = Flags());
    MatchIterator finditer(std::string pattern, std::string text,
                       Flags flags = Flags());

    Match search(std::string pattern, std::string text, Flags flags = Flags());
    Match match(std::string pattern, std::string text, Flags flags = Flags());
    Match fullmatch(std::string pattern, std::string text, Flags flags = Flags());
  };
} // end namespace library_interface

} // end namespace rematch

#endif
