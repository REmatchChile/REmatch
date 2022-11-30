#ifndef REMATCH_HPP
#define REMATCH_HPP

#include <string_view>

#include "regex/regex.hpp"

namespace rematch {

MatchIterator SearchIter(std::string_view s, rematch::RegEx e, rematch::RegExOptions opts = {});

} // end namespace rematch

#endif // REMATCH_HPP