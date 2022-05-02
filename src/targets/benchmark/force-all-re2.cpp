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

  std::ofstream logfile("logs/re2_force_all.log");

  re2::StringPiece supermatch, match;
  int count = 0;

  while(RE2::PartialMatch(input, pattern, &supermatch)) {
    count++;
    // logfile << "Supermatch: |" << supermatch.data() - doc.data() << ","
    //                  << supermatch.data() - doc.data() + supermatch.size()
    //                  << "> (\"" << supermatch << "\")\n";
    supermatch.remove_suffix(1);
    while(RE2::FullMatch(supermatch, pattern, &match)) {
      count++;
      // logfile << "\tSubmatch: |" << match.data() - doc.data() << ","
      //                << match.data() - doc.data() + match.size()
      //                << "> (\"" << match << "\")\n";

      supermatch.remove_suffix(supermatch.size() - match.size() + 1);
      }
    // logfile << '\n';
    // Jump to the start of the next match
    input.remove_prefix(supermatch.data() - input.data() + 1);
  }

  // std::cout << "Match: \"" << match << "\"\n";

  std::cout << count << '\n';

  logfile.close();

  return 0;
}