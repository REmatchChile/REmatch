#include <sys/resource.h>

#include "regex/regex.hpp"
#include "document.hpp"
#include "eval.hpp"
#include "util.hpp"

int main(int argc, char *argv[]) {
  rematch::RegEx rgx(".*!x{sparql[^\\n]*OPTIONAL[^\\n]*\\n}.*");

  std::unique_ptr<rematch::Match> match;

  const std::string doc = file2str("/home/nicovsj/repos/research/REmatch/REmatchEngine/datasets/RKBExplorer/sparql.log.1");

  rgx.feed(doc);

  size_t count = 0;

  while(match = rgx.internalFindIter()) {
    count++;
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
  std::cout << "Num of outputs: " << count << '\n';
}