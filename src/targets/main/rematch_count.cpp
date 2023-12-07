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

  std::string document = argv[1];
  std::string regex = argv[2];

  REMatch::Regex word_regex = REMatch::compile(regex, {.max_deterministic_states=100000});
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
