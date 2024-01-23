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
  std::unique_ptr<REMatch::MatchIterator> iterator = word_regex.finditer(document);
  auto match = iterator->next();
  while (match != nullptr) {
    match = iterator->next();
  }

  std::cout << "ExtendedVA: " << iterator->stats.extended_va_size << "\n"
            << "ExtendedDetVA: " << iterator->stats.extended_det_va_size << "\n"
            << "SearchDFA: " << iterator->stats.search_dfa_size << "\n"
            << "SearchNFA: " << iterator->stats.search_nfa_size << "\n"
            << "Nodes allocated: " << iterator->stats.nodes_allocated << "\n"
            << "Nodes used: " << iterator->stats.nodes_used << "\n"
            << "Nodes reused: " << iterator->stats.nodes_reused << "\n";

  return 0;
}
