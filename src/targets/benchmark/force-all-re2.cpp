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

  rgx = rematch::util::file2str(argv[2]);
	doc = rematch::util::file2str(argv[1]);

  re2::StringPiece input(doc);

  RE2 pattern(rgx);

  re2::StringPiece supermatch, match;
  int count = 0;
  size_t inp_sz = input.size(); // Original size

  for(size_t i=0; i < inp_sz; i++) {
    if(RE2::PartialMatch(input, pattern, &supermatch)) {
      size_t spmatch_size = supermatch.size();
      for(size_t j=0; j < spmatch_size; j++) {
        if(RE2::PartialMatch(supermatch, pattern, &match)) {
          count++;
          // std::cout << "|" << match.data() - doc.data() << ","
          //                  << match.data() - doc.data() + match.size() << ">\n";
        } else break;
          // No se puede asegurar que no haya output en el resto de los
          // prefijos.
        supermatch.remove_suffix(supermatch.size() - match.size() + 1);
      }
      // std::cout << "Match: \"" << match << "\"\n";

      // Jump to the start of the next match
      input.remove_prefix(supermatch.data() - input.data() + 1);
    } else break;
  }

  std::cout << count << '\n';
  return 0;
}