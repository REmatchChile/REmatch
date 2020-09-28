#include <sstream>

#include <sys/resource.h>

#include "regex/regex.hpp"
#include "document.hpp"
#include "eval.hpp"
#include "util.hpp"

int main(int argc, char *argv[]) {
  rematch::RegEx rgx(".* !x{\\w+} .*");

  std::unique_ptr<rematch::Match> match;

  // size_t count = 0;

  const std::string text{"This is a line\nthe second line is here\nwow, a third line!"};
  std::istringstream iss(text);
  std::string line;
  while(std::getline(iss, line, '\n')) {
    std::cout << "In line: \"" << line << "\"\n";
    rgx.feed(line);
    while(match = rgx.internalFindIter()) {
      std::cout << *match << std::endl;
    }
  }

  // GET MEMORY USAGE
	struct rusage usage;
	int ret;
	std::string memoryUsed;
	ret = getrusage(RUSAGE_SELF, &usage);
	if(ret == 0) {
		memoryUsed = formatMem(usage.ru_maxrss*1024);
	}

  std::cout << "Mem used: " << memoryUsed << '\n';
}