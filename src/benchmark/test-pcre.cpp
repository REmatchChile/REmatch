#include <iostream>

#include <pcrecpp.h>

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

  pcrecpp::RE re(rgx);
  pcrecpp::StringPiece input(doc);

  std::string match;

  int count = 0;
  while(re.FindAndConsume(&input, &match))
    count++;

  std::cout << count << '\n';

  return 0;
}






