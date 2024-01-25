#include "library_interface/rematch.hpp"

namespace REMatch {
inline namespace library_interface {

Regex compile(const std::string& pattern, Flags flags) {
  return Regex(pattern, flags);
}

std::unique_ptr<Match> findone(const std::string& pattern,
                               const std::string& document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.findone(document);
}

std::vector<Match> findall(const std::string& pattern,
                           const std::string& document, Flags flags) {
  Regex regex = compile(pattern, flags);
  std::unique_ptr<MatchIterator> iterator = regex.finditer(document);

  std::vector<Match> match_vector;
  std::unique_ptr<Match> output_match = iterator->next();
  while (output_match != nullptr) {
    match_vector.push_back(*output_match);
    output_match = iterator->next();
  }

  return match_vector;
}

std::unique_ptr<MatchIterator> finditer(const std::string& pattern, const std::string& document,
                       Flags flags) {
  std::string document_with_delimiters =
      rematch::add_start_and_end_chars(document);
  return std::make_unique<MatchIterator>(pattern, std::move(document_with_delimiters), document, flags);
}

}  // namespace library_interface
}  // namespace REMatch
