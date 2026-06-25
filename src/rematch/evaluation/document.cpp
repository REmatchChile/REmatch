#include "document.hpp"

#include "REmatch/span.hpp"
#include "evaluation/start_end_chars.hpp"

using namespace REmatch;

Document::Document(const std::string& str_) : str{str_} {}

const char& Document::operator[](std::string::size_type pos) const {
  return (pos >= str.size()) ? END_CHAR : str[pos];
}

std::string::size_type Document::size() const noexcept {
  return str.size() + 1;
}

std::string::size_type Document::find(std::string::value_type ch,
                                      std::string::size_type pos) const {
  return str.find(ch, pos);
}

std::string Document::substr(std::string::size_type pos, std::string::size_type count) const {
  return str.substr(pos, count);
}

std::string_view Document::get_segment(Span& span) const {
  return std::string_view(str).substr(span.first, span.second - span.first);
}
