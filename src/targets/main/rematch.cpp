#include "library_interface/rematch.hpp"
#include <iostream>
#include <string_view>

int main() {
  std::string pattern = "!a{a+}!b{b+}";
  std::string document = "aabb";
  REMatch::Flags flags;

  REMatch::Regex word_regex = REMatch::compile(pattern, flags);
  std::unique_ptr<REMatch::MatchIterator> iterator =
      word_regex.finditer(document);
  auto match = iterator->next();
  auto variables = iterator->variables();
  while (match != nullptr) {
    for (auto& variable : variables) {
      std::cout << variable << ": \"" << match->group(variable) << "\"" << std::endl;
    }
    match = iterator->next();
  }

  return 0;
}
