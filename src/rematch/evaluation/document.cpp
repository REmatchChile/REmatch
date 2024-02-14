#include "document.hpp"

#include <string>

using namespace rematch;

Document::Document(std::string_view str_) : str{str_} {}

const char& Document::operator[](std::string_view::size_type pos) const {
  return (pos < 1) ? START_CHAR : (pos > str.size()) ? END_CHAR : str[pos - 1];
}

std::string_view::size_type Document::size() const noexcept {
  return str.size() + 2;
}

std::string_view::size_type Document::find(
    std::string_view::value_type ch, std::string_view::size_type pos) const {
  // Offset by 1 to account the START_CHAR. Must check for npos to prevent overflows
  const auto res = str.find(ch, pos);
  return res != std::string_view::npos ? res + 1 : res;
}

std::string Document::substr(std::string_view::size_type pos,
                             std::string_view::size_type count) const {
  return std::string(str, pos, count);
}