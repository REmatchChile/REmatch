#include <iostream>

#include <boost/regex.hpp>

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

  boost::regex pattern(rgx);

  boost::match_results<std::string::const_iterator> matches;
  auto start = doc.cbegin();

  // std::cout << "Pattern: \"" << rgx << "\"\n";

  auto flags = boost::match_default;

  // Make '.' not match a new line
  flags |= boost::match_not_dot_newline;

  int count = 0;

  std::ofstream dump(".tmp/boost-output.log");

  while(regex_search(start, doc.cend(), matches, pattern, flags)) {
    dump << "|" << matches.position() << ',' << matches.length() - matches.position() << ">\n";
    // std::cout << "Match: \"" << matches.str() << "\"\n";
    start = matches[0].second;
    count++;
  }

  dump.close();

  std::cout << count << '\n';

  return 0;
}