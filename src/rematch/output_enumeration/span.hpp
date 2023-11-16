#ifndef SPAN_HPP
#define SPAN_HPP
#include <utility>
#include <cstdint>
#include <ostream>

using Span = std::pair<int64_t, int64_t>;

std::ostream& operator<<(std::ostream& os, const Span& span);

#endif
