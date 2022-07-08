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

  pcrecpp::RE re(rgx);
  pcrecpp::StringPiece input(doc);

  pcrecpp::StringPiece match;

  std::ofstream dump(".tmp/pcre-output.log");

  int count = 0;
  while (re.FindAndConsume(&input, &match)) {
    int ini = match.data() - doc.data();
    dump << "|" << ini << "," << ini + match.size() << ">\n";
    count++;
  }

  dump.close();

  std::cout << count << '\n';

  return 0;
}
