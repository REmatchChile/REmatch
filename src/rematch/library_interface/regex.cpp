#include "regex.hpp"

namespace REMatch {
inline namespace library_interface {

Regex::Regex(std::string_view pattern, Flags flags)
    : flags_(flags), regex_data_(rematch::get_regex_data(pattern, flags)) {}

std::unique_ptr<Match> Regex::find(std::string_view text) {
  MatchIterator iterator = finditer(text);
  return iterator.next();
}

MatchIterator Regex::finditer(std::string_view document_view) {
  std::string document = rematch::add_start_and_end_chars(document_view);

  return {regex_data_, std::move(document), flags_};
}

MatchIterator Regex::finditer(std::ifstream& file_document) {
  std::string document = rematch::read_file_and_add_start_and_end_chars(file_document);

  return {regex_data_, std::move(document), flags_};
}

} // end namespace library_interface
} // end namespace rematch
