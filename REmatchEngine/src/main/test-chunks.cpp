#include "regex/regex.hpp"
#include "document.hpp"
#include "eval.hpp"
#include "util.hpp"

int main(int argc, char *argv[]) {
  ChunkDocument doc;
  rematch::RegEx rgx(".*!x{a+}.*");
  rematch::Evaluator eval(rgx, doc);

  std::unique_ptr<rematch::Match> match;

  size_t count = 0;

  std::vector<std::string> chunks{"aaa", "aaabaa", "aaac", "aaaa", "aaaaa"};

  for(auto &chunk: chunks) {
    rgx.feed(chunk);
    std::cout << "chunk: " << chunk << '\n';
    while(match = rgx.internalFindIter()) {
      std::cout << *match << "\n";
      count++;
    }
  }

  std::cout << "Count: " << count << '\n';
}