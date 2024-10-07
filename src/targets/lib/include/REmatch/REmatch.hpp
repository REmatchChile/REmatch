#pragma once

#include <memory>
#include <string>
#include <vector>

#include "flags.hpp"
#include "match_iterator.hpp"

namespace REmatch {

inline namespace library_interface {
class Match;
class Query;

Query compile(const std::string& pattern, Flags flags = Flags());

std::unique_ptr<Match> findone(const std::string& pattern,
                               const std::string& document,
                               Flags flags = Flags());

std::vector<Match> findall(const std::string& pattern,
                           const std::string& document, Flags flags = Flags());

std::unique_ptr<MatchIterator> finditer(const std::string& pattern,
                                        const std::string& document,
                                        Flags flags = Flags());

}  // namespace library_interface
}  // namespace REmatch
