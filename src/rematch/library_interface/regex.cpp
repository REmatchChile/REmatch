#include "regex.hpp"

namespace REMatch {
inline namespace library_interface {

Regex::Regex(const std::string& pattern, Flags flags)
    : flags_(flags), regex_data_(rematch::get_regex_data(pattern, flags)) {}

std::unique_ptr<Match> Regex::findone(const std::string& text) {
  std::unique_ptr<MatchIterator> iterator = finditer(text);
  return iterator->next();
}

std::unique_ptr<MatchIterator> Regex::finditer(const std::string& document_view) {
  std::string document = rematch::add_start_and_end_chars(document_view);

  return std::make_unique<MatchIterator>(regex_data_, std::move(document),
                                         flags_);
}

} // end namespace library_interface
} // end namespace rematch
