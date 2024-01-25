#include "library_interface/rematch.hpp"
#include <string_view>
#include <iostream>


int main() {
  std::string document = "This is a document";
  std::string pattern = "!x{doc|document}";
  REMatch::Regex word_regex = REMatch::compile(pattern, REMatch::Flags());
  std::unique_ptr<REMatch::MatchIterator> iterator = word_regex.finditer(document);
  auto match = iterator->next();
  while (match != nullptr) {
    std::cout << "Span: <" << match->start("x") << ',' << match->end("x") << '>'
              << std::endl;
    match = iterator->next();
  }
  return 0;
}
