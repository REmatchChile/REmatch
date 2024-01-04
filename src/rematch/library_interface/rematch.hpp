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

Regex compile(std::string& pattern, Flags flags = Flags());
Regex compile(std::ifstream& pattern, Flags flags = Flags());

std::unique_ptr<Match> find(std::string& pattern, std::string& document, Flags flags = Flags());
std::vector<Match> findall(std::string& pattern, std::string& document,
                        Flags flags = Flags());
MatchIterator finditer(std::string& pattern, std::string& document,
                    Flags flags = Flags());

std::unique_ptr<Match> search(std::string& pattern, std::string& document, Flags flags = Flags());
std::unique_ptr<Match> match(std::string& pattern, std::string& document, Flags flags = Flags());
std::unique_ptr<Match> fullmatch(std::string& pattern, std::string& document, Flags flags = Flags());

}
}

#endif
