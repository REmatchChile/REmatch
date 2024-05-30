#include "library_interface/rematch.hpp"
#include <cstdlib>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <pattern> <document>" << std::endl;
    return EXIT_FAILURE;
  }
  std::string pattern = argv[1];
  std::string document = argv[2];

  REMatch::Flags flags;
  REMatch::Query query = REMatch::compile(pattern, flags);
  std::unique_ptr<REMatch::MatchIterator> iterator = query.finditer(document);

  auto match = iterator->next();
  auto variables = iterator->variables();
  while (match != nullptr) {
    for (auto& variable : variables)
      std::cout << variable << ": \"" << match->group(variable) << "\" |" << match->start(variable) << "," << match->end(variable) <<  ">" << std::endl;
    match = iterator->next();
  }

  return EXIT_SUCCESS;
}
