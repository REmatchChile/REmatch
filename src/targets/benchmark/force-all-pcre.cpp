#include <iostream>

#include <pcrecpp.h>

#include "util/util.hpp"

int main(int argc, char const *argv[]) {

  if(argc != 3) {
		std::cerr << "Error parsing args.\nUsage:\n\t" << argv[0] <<
			" [rgx_file] [doc_file]\n";
		exit(1);
	}

  std::string doc, rgx;

  rgx = rematch::util::file2str(argv[2]);
	doc = rematch::util::file2str(argv[1]);

  pcrecpp::StringPiece input(doc);

  pcrecpp::RE pattern(rgx);

  pcrecpp::StringPiece supermatch, match;
  int count = 0;
  size_t inp_sz = input.size(); // Original size

  for(size_t i=0; i < inp_sz; i++) {
    if(pattern.PartialMatch(input, &supermatch)) {
      size_t spmatch_size = supermatch.size();
      for(size_t j=0; j < spmatch_size; j++) {
        if(pattern.FullMatch(supermatch, &match)) {
          count++;
          // std::cout << "|" << match.data() - doc.data() << ","
          //                  << match.data() - doc.data() + match.size() << ">\n";
        }
        supermatch.remove_suffix(1);
      }
      // std::cout << "Match: \"" << match << "\"\n";

      // Jump to the start of the next match
      input.remove_prefix(supermatch.data() - input.data() + 1);
    } else break;
  }

  std::cout << count << '\n';
  return 0;
}