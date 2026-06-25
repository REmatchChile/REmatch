#include "line.hpp"

#include "evaluation/start_end_chars.hpp"

using namespace REmatch;

Line::Line(std::string_view str_) : str{str_} {}

const char& Line::operator[](std::string::size_type pos) const {
  return (pos >= str.size()) ? END_CHAR : str[pos];
}

std::string::size_type Line::size() const noexcept {
  return str.size() + 1;
}
