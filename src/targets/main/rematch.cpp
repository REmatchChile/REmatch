#include "library_interface/rematch.hpp"
#include <string_view>
#include <iostream>


int main() {
  std::string_view document = "This is a document";
  REMatch::Regex word_regex = REMatch::compile("!x{doc|document}", REMatch::Flags());
  REMatch::MatchIterator iterator = word_regex.finditer(document);
  auto match = iterator.next();
  while (match != nullptr) {
    std::cout << "Span: <" << match->start("x") << ',' << match->end("x") << '>'
              << std::endl;
    match = iterator.next();
  }
  return 0;
}
