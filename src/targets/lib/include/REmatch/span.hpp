#pragma once

#include <cstdint>
#include <ostream>
#include <utility>

using Span = std::pair<int64_t, int64_t>;

std::ostream& operator<<(std::ostream& os, const Span& span);
