#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include "library_interface/rematch.hpp"

int main(int argc, char* argv[]) {
  ZoneScoped;
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <regexfile> <documentfile>\n";
    return 1;
  }

  std::ifstream document(argv[1]);
  std::ifstream regex(argv[2]);

  if (!document.is_open() || !regex.is_open()) {
    std::cerr << "Error loading file\n";
    return 1;
  }

  REMatch::Flags flags{false, false, 12, 100000};

  REMatch::Regex word_regex = REMatch::compile(regex, flags);
  REMatch::MatchIterator iterator = word_regex.finditer(document);
  auto match = iterator.next();
  size_t matches_count = 0;
  while (match != nullptr) {
    matches_count ++;
    match = iterator.next();
  }
  std::cout << matches_count << std::endl;
  return 0;
}
