#include "library_interface/rematch.hpp"

namespace REMatch {
inline namespace library_interface {

Regex compile(const std::string& pattern, Flags flags) {
  return Regex(pattern, flags);
}

Regex compile(std::ifstream& pattern_file, Flags flags) {
  std::string pattern = rematch::read_file(pattern_file);
  return Regex(pattern, flags);
}

std::unique_ptr<Match> find(const std::string& pattern, const std::string& document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::vector<Match> findall(const std::string& pattern, const std::string& document,
                        Flags flags) {
  Regex regex = compile(pattern, flags);
  MatchIterator iterator = regex.finditer(document);

  std::vector<Match> match_vector;
  std::unique_ptr<Match> output_match = iterator.next();
  while (output_match != nullptr) {
    match_vector.push_back(*output_match);
    output_match = iterator.next();
  }

  return match_vector;
}

MatchIterator finditer(const std::string& pattern, const std::string& document,
                    Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.finditer(document);
}

std::unique_ptr<Match> search(const std::string& pattern, const std::string& document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::unique_ptr<Match> match(const std::string& pattern, const std::string& document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

std::unique_ptr<Match> fullmatch(const std::string& pattern, const std::string& document, Flags flags) {
  Regex regex = compile(pattern, flags);
  return regex.find(document);
}

}
}
