#include "library_interface/rematch.hpp"

namespace REMatch {
inline namespace library_interface {

Regex compile(std::string_view pattern, Flags flags) {
  return Regex(pattern, flags);
}

std::optional<Match> find(std::string_view pattern, std::string_view document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::vector<Match> findall(std::string_view pattern, std::string_view document,
                        Flags flags) {
  Regex regex = compile(pattern, flags);
  MatchIterator iterator = regex.finditer(document, flags);

  std::vector<Match> output;
  while (iterator.has_next()) {
    output.push_back(iterator.next());
  }
}

MatchIterator finditer(std::string_view pattern, std::string_view document,
                    Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.finditer(pattern, flags);
}

std::optional<Match> search(std::string_view pattern, std::string_view document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::optional<Match> match(std::string_view pattern, std::string_view document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::optional<Match> fullmatch(std::string_view pattern, std::string_view document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

}
}
