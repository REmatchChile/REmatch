#include <iostream>

#include <boost/regex.hpp>

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

  boost::regex pattern(rgx);

  boost::smatch matches, submatches;
  auto start = doc.cbegin();

  // std::cout << "Pattern: \"" << rgx << "\"\n";

  auto flags = boost::match_default;

  // Make '.' not match a new line
  flags |= boost::match_not_dot_newline;

  int count = 0;

  std::ofstream dump("logs/boost-output.log");

  while (boost::regex_search(start, doc.cend(), matches, pattern, flags)) {
    dump << "|" << matches.position() << ',' << matches.length() -
    matches.position() << ">\n";
    auto mstart = matches[0].first, mend = matches[0].second;
    mend--;
    count++;
    while (boost::regex_search(mstart, mend, submatches, pattern, flags)) {
      count++;
      dump << "|" << submatches.position() << ',' << submatches.length() -
      submatches.position() << ">\n";
      mend = submatches[0].second;
      mend--;
    }
    start = matches[0].first;
    start++;
  }

  dump.close();

  std::cout << count << '\n';

  return 0;
}