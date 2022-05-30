#include <iostream>

#include <pcrecpp.h>

#include "util/util.hpp"

int main(int argc, char const *argv[]) {

  if (argc != 3) {
    std::cerr << "Error parsing args.\nUsage:\n\t" << argv[0]
              << " [rgx_file] [doc_file]\n";
    exit(1);
  }

  std::string doc, rgx;

  rgx = rematch::util::file2str(argv[2]);
  doc = rematch::util::file2str(argv[1]);

  pcrecpp::StringPiece input(doc);

  pcrecpp::RE pattern(rgx);

  pcrecpp::StringPiece supermatch, match;
  int count = 0;

  while (pattern.PartialMatch(input, &supermatch)) {
    count++;
    supermatch.remove_suffix(1);
    while (pattern.PartialMatch(supermatch, &match)) {
      count++;
      // std::cout << "|" << match.data() - doc.data() << ","
      //                  << match.data() - doc.data() + match.size() << ">\n";
      supermatch.remove_suffix(supermatch.size() - match.size() + 1);
    }
    input.remove_prefix(supermatch.data() - input.data() + 1);
  }

  std::cout << count << '\n';
  return 0;
}