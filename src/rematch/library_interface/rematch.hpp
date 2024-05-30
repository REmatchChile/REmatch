#ifndef LIBRARY_INTERFACE__REMATCH_HPP
#define LIBRARY_INTERFACE__REMATCH_HPP

#include <optional>
#include <string>
#include <vector>

#include "library_interface/flags.hpp"
#include "library_interface/match.hpp"
#include "library_interface/match_iterator.hpp"
#include "library_interface/query.hpp"

namespace REMatch {
inline namespace library_interface {

Query compile(const std::string& pattern, Flags flags = Flags());

std::unique_ptr<Match> findone(const std::string& pattern,
                               const std::string& document,
                               Flags flags = Flags());
std::vector<Match> findall(const std::string& pattern,
                           const std::string& document, Flags flags = Flags());
std::unique_ptr<MatchIterator> finditer(const std::string& pattern, const std::string& document,
                       Flags flags = Flags());

}  // namespace library_interface
}  // namespace REMatch

#endif
