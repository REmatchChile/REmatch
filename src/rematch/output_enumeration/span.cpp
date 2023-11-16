#include "span.hpp"

std::ostream& operator<<(std::ostream& os, const Span& span) {
  os << "[" << span.first << ", " << span.second << "]" << std::endl;
  return os;
}
