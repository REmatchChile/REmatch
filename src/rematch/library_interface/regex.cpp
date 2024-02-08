#include "regex.hpp"

#include "evaluation/document.hpp"

using namespace rematch;

namespace REMatch {
inline namespace library_interface {

Regex::Regex(std::string_view pattern, Flags flags)
    : flags_(flags), regex_data_(rematch::get_regex_data(pattern, flags)) {}

std::unique_ptr<Match> Regex::findone(std::string_view text) {
  std::unique_ptr<MatchIterator> iterator = finditer(text);
  return iterator->next();
}

std::unique_ptr<MatchIterator> Regex::finditer(std::string_view str) {
  return std::make_unique<MatchIterator>(regex_data_, str, flags_);
}

} // end namespace library_interface
} // end namespace rematch
