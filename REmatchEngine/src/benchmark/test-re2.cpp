#include <iostream>

#include <re2/re2.h>

#include "../util.hpp"

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

  std::string match;

  int count = 0;
  while(RE2::FindAndConsume(&input, pattern, &match))
    count++;

  std::cout << count << '\n';

  return 0;
}