#include "document.hpp"

#include "evaluation/start_end_chars.hpp"

using namespace REmatch;

Document::Document(const std::string& str_) : str{str_} {}

const char& Document::operator[](std::string::size_type pos) const {
  return (pos < 1) ? START_CHAR : (pos > str.size()) ? END_CHAR : str[pos - 1];
}

std::string::size_type Document::size() const noexcept {
  return str.size() + 2;
}

std::string::size_type Document::find(std::string::value_type ch,
                                      std::string::size_type pos) const {
  // Offset by 1 to account the START_CHAR. Must check for npos to prevent overflows
  const auto res = str.find(ch, pos);
  return res != std::string::npos ? res + 1 : res;
}

std::string Document::substr(std::string::size_type pos,
                             std::string::size_type count) const {
  return str.substr(pos, count);
}