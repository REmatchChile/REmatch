#include "library_interface/rematch.hpp"
#include <iostream>
#include <string_view>

int main() {
  std::string pattern = R"((,|^)!user{\w+}@!domain{\w+\.\w+}(,|$))";
  std::string document = "vecalisto@uc.cl,test@gmail.com,jejjee@jaja.cl";
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
