#include "regex/regex.hpp"
#include "document.hpp"
#include "eval.hpp"
#include "util.hpp"

int main(int argc, char *argv[]) {
  rematch::RegEx rgx(".*!x{a+}.*");
  const std::string doc = "aab";

  rematch::Match_ptr match;

  int count = 0;

  while(match = rgx.findIter(doc)) {
    std::cout << *match << '\n';
    count++;
  }

  std::cout << "Count: " << count << '\n';
}