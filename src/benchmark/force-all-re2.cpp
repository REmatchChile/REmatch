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

  re2::StringPiece match;
  int count = 0;
  size_t inp_sz = input.size(); // Original size

  for(size_t i=0; i < inp_sz; i++) {
    for(size_t j=1; i+j <= inp_sz; j++) {
      re2::StringPiece subinput = input.substr(i, j);
      if(RE2::PartialMatch(subinput, pattern, &match)) {
        count++;
        std::cout << "Match: \"" << match << "\"\n";
      }
    }
  }

  std::cout << count << '\n';
  return 0;
}