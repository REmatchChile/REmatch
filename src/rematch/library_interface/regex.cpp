#include "regex.hpp"

namespace REMatch {
inline namespace library_interface {

Regex::Regex(const std::string& pattern, Flags flags)
    : flags_(flags), regex_data_(rematch::get_regex_data(pattern, flags)) {}

std::unique_ptr<Match> Regex::findone(const std::string& text) {
  MatchIterator iterator = finditer(text);
  return iterator.next();
}

MatchIterator Regex::finditer(const std::string& document_view) {
  std::string document = rematch::add_start_and_end_chars(document_view);

  return {regex_data_, std::move(document), document_view, flags_};
}

MatchIterator Regex::finditer(const std::string& text, Flags flags) {
  return finditer(std::string_view(text), flags);
}

} // end namespace library_interface
} // end namespace rematch
