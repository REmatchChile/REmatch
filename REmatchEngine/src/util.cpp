#include "util.hpp"

#include <string>
#include <sstream>
#include <istream>
#include <fstream>

std::string file2str(std::string filename) {
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

std::string formatMem(size_t sizeInBytes) {
	std::stringstream ss;
	double sizeForm = (double)sizeInBytes;
	std::string units[4] = {"", "K", "M", "G"};
	int counter = 0;

	while(sizeForm > 1024) {
		sizeForm /= 1024;
		counter++;
	}

	ss << sizeForm << units[counter];

	return ss.str();
}