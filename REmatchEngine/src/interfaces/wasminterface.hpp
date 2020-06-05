#ifndef WASMINTERFACE_HPP
#define WASMINTERFACE_HPP

#include <string>
#include <sstream>
#include <vector>
#include <utility>

#include "evaluation.hpp"

class Evaluation;

struct pair_ {
  public:
    unsigned int x, y;

    pair_(): x(-1), y(-1) {}
    pair_(unsigned int x, unsigned int y): x(x), y(y) {}
	pair_(std::pair<size_t,size_t> p): x(p.first), y(p.second) {}
    void setx(unsigned int x) {
  		this->x = x;
	}
    void sety(unsigned int y) {
		this->y = y;
	}
};

class WasmInterface {

	typedef std::map<std::string, std::pair<size_t, size_t>> mapping_type;

	public:
		WasmInterface(std::string doc, std::string regex);
		std::vector<std::string> getOutputSchema();
		void init();
		std::vector<pair_> next();
		bool hasNext();
		std::vector<pair_> pairConvert(mapping_type mapping);

	private:
		std::istringstream *documentStream;
		std::string regex;
        Evaluation *evaluation;
};

#endif