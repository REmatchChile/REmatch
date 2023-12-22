#include "library_interface/rematch.hpp"

namespace REMatch {
inline namespace library_interface {

Regex compile(std::string_view pattern, Flags flags) {
  return Regex(pattern, flags);
}

std::unique_ptr<Match> find(std::string_view pattern, std::string_view document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::vector<Match> findall(std::string_view pattern, std::string_view document,
                        Flags flags) {
  Regex regex = compile(pattern, flags);
  MatchIterator iterator = regex.finditer(document, flags);

  std::vector<Match> match_vector;
  std::unique_ptr<Match> output_match = iterator.next();
  while (output_match != nullptr) {
    match_vector.push_back(*output_match);
    output_match = iterator.next();
  }

  return match_vector;
}

MatchIterator finditer(std::string_view pattern, std::string_view document,
                    Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.finditer(document, flags);
}

std::unique_ptr<Match> search(std::string_view pattern, std::string_view document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::unique_ptr<Match> match(std::string_view pattern, std::string_view document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::unique_ptr<Match> fullmatch(std::string_view pattern, std::string_view document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

}
}
