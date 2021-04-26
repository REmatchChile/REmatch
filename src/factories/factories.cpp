#include <iostream>
#include <unordered_map>
#include <vector>
#include <ostream>
#include <bitset>
#include <unordered_set>

#include "factories.hpp"
#include "charclass.hpp"
#include "parser/visitors.hpp"
#include "parser/exceptions.hpp"

namespace rematch {

VariableFactory :: VariableFactory()
	:numVars(0) {}

std::string VariableFactory :: getVarName(unsigned int position) {
	return varMap[position];
}

void VariableFactory :: addVar(std::string varName) {

	if(codeMap.count(varName)) {
		std::cerr << "Key " << varName << " already in.\n";
		return;
	}

	if(!(numVars < MAX_VARS)) {
		std::cerr << "Maximum number of variables reached. (" << MAX_VARS << ").\n";
		return;
	}

	codeMap[varName] = numVars;
	varMap[numVars] = varName;

	// Add offsetMap new entries, defaults to zero.
	offsetMap.push_back(0);
	offsetMap.push_back(0);

	numVars++;
}

std::bitset<32> VariableFactory :: getOpenCode(std::string varName) {
	std::bitset<32> bitstring;

	auto it = codeMap.find(varName);

	if(it != codeMap.end()) {
		bitstring.set(it->second * 2);
	}

	return bitstring;
}

std::bitset<32> VariableFactory::getCloseCode(std::string varName) {
	std::bitset<32> bitstring;
	auto it = codeMap.find(varName);


	if(it != codeMap.end()) {
		bitstring.set(it->second * 2+1);
	}

	return bitstring;
}

std::vector<std::string> VariableFactory::getOutputSchema() {
	std::vector<std::string> ret(numVars, "");
	for (unsigned int i = 0; i < numVars; i++) {
		ret[i] = varMap[i];
	}
	return ret;
}

std::string VariableFactory :: getVarUtil(std::bitset<32> code) {
	std::stringstream ss;

	// Get a container just for printing output correctly with commas
	std::vector<std::string> container;

	for(size_t i=0; i < 16; i++) {
		if(code[2*i]) {
			ss << varMap[i];
			if(offsetMap[2*i]) ss << "(-" << offsetMap[2*i] << ")";
			ss << "<";
			container.push_back(ss.str());
			ss.str(std::string()); // Erase the stringstream
		}
		if(code[2*i+1]) {
			ss << ">"  << varMap[i];
			if(offsetMap[2*i+1]) ss << "(-" << offsetMap[2*i+1] << ")";
			container.push_back(ss.str());
			ss.str(std::string()); // Erase the stringstream
		}

	}

	if(!container.empty()) {
		ss << container[0];
		for(size_t i=1; i < container.size(); i++) {
			ss << "" << container[i];
		}
	}

	return ss.str();
}

std::string VariableFactory::pprint() {
	std::stringstream ss;
	for(auto &it: varMap) {
		ss << it.first  << " -> " << it.second << "\n";
	}

	return ss.str();
}

void VariableFactory :: merge(VariableFactory &rhs) {
	for(auto &it: rhs.codeMap) {
		if (!codeMap.count(it.first)) {
			assert(numVars < MAX_VARS);
			codeMap[it.first] = numVars;
			varMap[numVars] = it.first;

			offsetMap.push_back(0);
			offsetMap.push_back(0);

		  numVars++;
		} else {
			throw parsing::BadRegex("Regex is not functional.");
		}
	}

}

bool VariableFactory::isMember(std::string varName) {
	return (bool)codeMap.count(varName);
}

bool VariableFactory::isEmpty() {
	return (bool)(numVars == 0);
}

bool VariableFactory::operator ==(const VariableFactory &vf) const {
	if(numVars != vf.numVars) return false;

	for(auto &it: codeMap) {
		if(!vf.codeMap.count(it.first)) return false; // If one key is not present in both
	}
	return true;
}



FilterFactory::FilterFactory(): numFilters(0) {}

std::string FilterFactory::pprint() {
	std::stringstream ss;

	for(auto &elem: codeMap) {
		ss << elem.first.label << " -> " << elem.second << "\n";
	}

	return ss.str();
}


void FilterFactory :: addFilter(CharClass cs) {
	if(!codeMap.count(cs)) {
		codeMap[cs] = numFilters;
		filterMap[numFilters] = cs;
		numFilters++;
	}
}

int FilterFactory :: getCode(CharClass cs) {
	return codeMap[cs];
}

CharClass FilterFactory :: getFilter(int code) {
	return filterMap[code];
}

bool FilterFactory :: isMember(CharClass cs) {
	if(codeMap.count(cs)) return true;
	return false;
}

void FilterFactory :: merge(FilterFactory &rest) {
	/**
	* Merges the FilterFactory with another one inplace. Used at parsing
	* post-processing.
	*/

	for(auto &it : rest.filterMap) {
		assert(!isMember(it.second)); // Check for functional regex

		filterMap[numFilters] = it.second;
		codeMap[it.first] = numFilters;
		numFilters++;

	}
}

std::unordered_map<BitsetWrapper, std::vector<char>>  FilterFactory :: allPossibleCharBitsets() {
	std::unordered_map<BitsetWrapper, std::vector<char>> ret;
	for(char a = (char)32; a < 127; a++) {
    BitsetWrapper bs = applyFilters(a);
    auto it = ret.find(bs);
    if(it == ret.end())
      ret.insert(std::make_pair(bs, std::vector<char>(1, a)));
    else
      it->second.push_back(a);
	}

	return ret;

}

bool FilterFactory :: inIntersection(char a, BitsetWrapper bs) {
	for(size_t i=0; i < bs.size(); i++) {
		if(bs.get(i) && !getFilter(i).check(a))
			return false;
	}
	return bs.any();
}


BitsetWrapper FilterFactory :: applyFilters(char a) {
	/**
	* Returns the bitset vector corresponding to the character after applying
	* all the filters. Searches in bitsetMap hash table. Computes and stores
	* the bitset if not found.
	*
	* Example:
	* Suppose there are 3 filters: "a", "[0-9ab]", "[abc]" with corresponding
	*	codes 0, 1, 2. Then the following table shows the returned bitsets for
	*	different chars
	*
	*	 	CHAR 		 RETURNED BITSET
	*	 		a  					111
	*	 		b 					011
	*	 		c 					001
	*	 		1					010
	*	 		(					000
	*/


	auto it = bitsetMap.find(a);

	if(it == bitsetMap.end()) {
		BitsetWrapper bitsetVector(numFilters);
		for(auto &it: filterMap) {
			bitsetVector.set(it.first, it.second.check(a));
		}
		bitsetMap.insert(std::make_pair(a, bitsetVector));

		return bitsetVector;
	}


	return it->second;

}

} // end namespace rematch
