#include <iostream>

#include <re2/re2.h>

#include "util/util.hpp"

int main(int argc, char const *argv[]) {

  if(argc != 3) {
		std::cerr << "Error parsing args.\nUsage:\n\t" << argv[0] <<
			" [rgx_file] [doc_file]\n";
		exit(1);
	}

  std::string doc, rgx;

  rgx = file2str(argv[2]);
	doc = file2str(argv[1]);

  re2::StringPiece input(doc);

  RE2 pattern(rgx);

  re2::StringPiece match;

  int count = 0;

  std::ofstream dump(".tmp/re2-output.log");

  while(RE2::FindAndConsume(&input, pattern, &match)) {
    int ini = match.data() - doc.data();
    dump << "|" << ini << "," << ini + match.size() << ">\n";
    count++;
  }

  dump.close();

  std::cout << count << '\n';

  return 0;
}