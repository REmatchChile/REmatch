#include "library_interface/rematch.hpp"
#include <string_view>
#include <iostream>


int main(int argc, char *argv[]) {
  std::string_view document = "This is a document";
  REMatch::Regex word_regex = REMatch::compile("[a-zA-Z]*", REMatch::Flags());
  REMatch::MatchIterator iterator = word_regex.finditer(document);
  while (iterator.has_next()) {
    REMatch::Match match = iterator.next();
    std::cout << "Span: <" << match.get_span().first << ',' <<
                              match.get_span().second << '>' <<
        std::endl;
  }
  return 0;
}
