#include <iostream>

#include <tre/tre.h>

#include "../util.hpp"

int main(int argc, char const *argv[]) {

  if(argc != 3) {
		std::cerr << "Error parsing args.\nUsage:\n\t" << argv[0] <<
			" [rgx_file] [doc_file]\n";
		exit(1);
	}

  std::string doc, rgx;

	doc = file2str(argv[1]);
  rgx = file2str(argv[2]);

  regex_t pattern;

  tre_regcomp(&pattern, rgx.c_str(), REG_NEWLINE | REG_EXTENDED);

  regmatch_t pmatch[1] = {{0,0}};

  auto beg = doc.c_str();

  int count = 0;
  std::ofstream dump(".tmp/tre-output.log");
  while(tre_regexec(&pattern, beg, 1, pmatch, 0) == 0) {
    dump << "|" << pmatch[0].rm_so << ',' << pmatch[0].rm_eo << ">\n";
    beg += pmatch[0].rm_eo;
    count++;
  }

  dump.close();

  std::cout << count << '\n';

  return 0;
}