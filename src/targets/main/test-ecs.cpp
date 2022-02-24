#include <iostream>
#include <string>

#include "parse/parser.hpp"
#include "automata/nfa/lva.hpp"

using namespace rematch;

int main() {

  const std::string pattern(".*(!x{.}|!x{..+}).*");

  std::unique_ptr<LogicalVA> A = regex2LVA(pattern);

  std::cout << "Original VA:\n";
  std::cout << *A << std::endl;

  LogicalVA B(*A);

  std::cout << "Copied VA:\n";
  std::cout << B << std::endl;

  return 0;
}