#include <pcrecpp.h>
#include <cli.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{

	if(argc != 3) {
		std::cerr << "Error parsing args.\nUsage:\n\t" << argv[0] << 
			" [rgx_file] [doc_file]\n";
		exit(1);
	}

	std::string rgx, doc;

	rgx = file2str(argv[1]);
	doc = file2str(argv[2]);

	std::string result;
	pcrecpp::RE re(rgx);

	pcrecpp::StringPiece input(doc);

  std::string match;

  int count = 0;
  while (re.FindAndConsume(&input, &match)) {
    count++;
  }

  std::cout << count << '\n';

	return 0;
}