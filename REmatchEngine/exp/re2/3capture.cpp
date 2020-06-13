#include <re2/re2.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <istream>
#include <string>

inline std::string file2str(std::string filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, in.end);
		contents.resize(in.tellg());
		in.seekg(0, in.beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw std::runtime_error("Error loading file");
}


int main(int argc, char const *argv[])
{

	if(argc != 3) {
		std::cerr << "Error parsing args.\nUsage:\n\t" << argv[0] << 
			" [rgx_file] [doc_file]\n";
		exit(1);
	}

	std::string rgx, doc;

	rgx = file2str(argv[2]);
	doc = file2str(argv[1]);

	re2::StringPiece input(doc);

	// std::cout << "DOC: " << input << "\n";

  	std::string match1, match2, match3;

	int count = 0;
	while (RE2::FindAndConsume(&input, rgx, &match1, &match2, &match3)) {
		// std::cout << match << "\n";
		count++;
	}

	std::cout << count << "\n";

	return 0;
}