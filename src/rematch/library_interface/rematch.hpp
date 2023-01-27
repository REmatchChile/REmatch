#ifndef LIBRARY_INTERFACE__REMATCH_HPP
#define LIBRARY_INTERFACE__REMATCH_HPP

#include <vector>
#include <string>
#include <optional>

#include "library_interface/match.hpp"
#include "library_interface/flags.hpp"
#include "library_interface/match_iterator.hpp"
#include "library_interface/regex.hpp"

namespace REMatch {
inline namespace library_interface {

Regex compile(std::string_view pattern, Flags flags = Flags());

std::optional<Match> find(std::string_view pattern, std::string_view document, Flags flags = Flags());
std::vector<Match> findall(std::string_view pattern, std::string_view document,
                        Flags flags = Flags());
MatchIterator finditer(std::string_view pattern, std::string_view document,
                    Flags flags = Flags());

std::optional<Match> search(std::string_view pattern, std::string_view document, Flags flags = Flags());
std::optional<Match> match(std::string_view pattern, std::string_view document, Flags flags = Flags());
std::optional<Match> fullmatch(std::string_view pattern, std::string_view document, Flags flags = Flags());

}
}

#endif
