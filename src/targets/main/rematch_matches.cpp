#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include "library_interface/rematch.hpp"

std::string get_string_from_file(std::string filename) {
  std::ifstream file_stream(filename, std::ios::in | std::ios::binary);
  if (file_stream.is_open()) {
    std::string contents;
    file_stream.seekg(0, file_stream.end);
    contents.resize(file_stream.tellg());
    file_stream.seekg(0, file_stream.beg);
    file_stream.read(&contents[0], contents.size());
    file_stream.close();
    return contents;
  }
  throw std::runtime_error("Error loading file");
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <regexfile> <documentfile>\n";
    return 1;
  }

  std::string document = get_string_from_file(argv[1]);
  std::string regex = get_string_from_file(argv[2]);

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
