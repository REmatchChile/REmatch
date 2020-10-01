#include <sstream>

#include "regex/regex.hpp"
#include "document.hpp"
#include "eval.hpp"
#include "util.hpp"

int main(int argc, char *argv[]) {
  ChunkDocument doc;
  rematch::RegEx rgx(".*!x{\\w+}.*");
  rematch::Evaluator eval(rgx, doc);

  std::unique_ptr<rematch::Match> match;

  // size_t count = 0;

  const std::string text{"This is a line\nthe second line is here\nwow, a third line!"};
  std::istringstream iss(text);
  std::string line;
  while(std::getline(iss, line, '\n')) {
    rgx.feed(line);
    while(match = eval.next()) {
      std::cout << *match << std::endl;
    }
  }
}