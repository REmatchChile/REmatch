#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include <fstream>

#include "cli.hpp"
#include "automata/lva.hpp"
#include "options.hpp"

class Interface {
  public:
	Interface(std::string docFile, std::string input, rematch::Options opt);

	// Algorithm runs
	void run();

  private:

	void normalRun();
	void benchmarkRun();

	// Utilities
	std::string formatMem(size_t sizeInBytes);
	std::string file2str(std::string filename);

	std::istream *document_stream_;
	std::string document_filename_;
	std::string pattern_;
	rematch::Options options_;
};

#endif
