#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include "library_interface/rematch.hpp"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <regexfile> <documentfile>\n";
    return 1;
  }

  std::ifstream document_file(argv[1]);
  std::ifstream regex_file(argv[2]);

  if (!document_file.is_open() || !regex_file.is_open()) {
    std::cerr << "Error loading file\n";
    return 1;
  }

  std::string document = rematch::read_file(document_file);
  std::string regex = rematch::read_file(regex_file);

  REMatch::Flags flags{false, false, 12, 100000};

  REMatch::Regex word_regex = REMatch::compile(regex, flags);
  REMatch::MatchIterator iterator = word_regex.finditer(document);
  auto match = iterator.next();
  size_t matches_count = 0;
  while (match != nullptr) {
    std::cout << *match << std::endl;
    matches_count++;
    match = iterator.next();
  }
  return 0;
}
