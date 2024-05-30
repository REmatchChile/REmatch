#include "library_interface/rematch.hpp"

namespace REMatch {
inline namespace library_interface {

Query compile(const std::string& pattern, Flags flags) {
  return Query(pattern, flags);
}

std::unique_ptr<Match> findone(const std::string& pattern,
                               const std::string& document, Flags flags) {
  Query query = compile(pattern, flags);
  return query.findone(document);
}

std::vector<Match> findall(const std::string& pattern,
                           const std::string& document, Flags flags) {
  Query query = compile(pattern, flags);
  std::unique_ptr<MatchIterator> iterator = query.finditer(document);

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
  return std::make_unique<MatchIterator>(pattern, document, flags);
}

}  // namespace library_interface
}  // namespace REMatch
