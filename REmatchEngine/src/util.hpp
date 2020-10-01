#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
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

#endif // UTIL_HPP
