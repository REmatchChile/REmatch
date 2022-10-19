#include <iostream>

#include <re2/re2.h>

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

  re2::StringPiece input(doc);

  RE2::Options opts;
  opts.set_log_errors(false);

  rematch::util::Timer t;

  RE2 pattern(rgx, opts);

  int ncapturegroups = pattern.NumberOfCapturingGroups();

  re2::StringPiece match;

  int count = 0;

  std::ofstream dump(".tmp/re2-output.log");

  // std::cout << "Input: \"" << input << "\"\n";
  // std::cout << "Pattern: \"" << rgx << "\"\n";

  while (RE2::FindAndConsume(&input, pattern, &match)) {
    for(int i = 0; i < 5; i++) {
      int ini = match.data() - doc.data();
      dump << "m" << (i+1) << " = |" << ini << "," << ini + match.size() << ">\t";
    }
    dump << '\n';
    count++;
  }

  dump.close();

  std::cout << count << '\n';

  return 0;
}