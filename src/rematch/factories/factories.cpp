#include <iostream>
#include <unordered_map>
#include <vector>
#include <ostream>
#include <bitset>
#include <unordered_set>
#include <algorithm>

#include "factories.hpp"
#include "charclass.hpp"
#include "parse/regex/visitors.hpp"
#include "parse/regex/exceptions.hpp"

namespace rematch {

VariableFactory::VariableFactory() {}

std::string VariableFactory::get_var(uint position) {
	return data_[position];
}

int VariableFactory::position(std::string var) const {
	auto it = std::lower_bound(data_.begin(), data_.end(), var);

	if(it != data_.end() && var >= *it) {
		return it - data_.begin();
	} else {
		// TODO: Throw exception
		return -1;
	}
}

void VariableFactory::add(std::string var) {
	if(size() >= MAX_VARS)
		return;

	auto it = std::lower_bound(data_.begin(), data_.end(), var);

	if(it == data_.end() || var < *it) {
		data_.insert(it, var);
		// Add offsetMap new entries, defaults to zero.
		offsetMap.push_back(0);
		offsetMap.push_back(0);
	}
}

std::bitset<32> VariableFactory::open_code(std::string var) {
	std::bitset<32> bitstring;

	auto it = std::lower_bound(data_.begin(), data_.end(), var);

	if(it != data_.end() && var >= *it) {
		bitstring.set((it - data_.begin()) * 2);
	}

	return bitstring;
}

std::bitset<32> VariableFactory::close_code(std::string var) {
	std::bitset<32> bitstring;

	auto it = std::lower_bound(data_.begin(), data_.end(), var);

	if(it != data_.end() && var >= *it) {
		bitstring.set((it - data_.begin()) * 2 + 1);
	}

	return bitstring;
}

std::string VariableFactory :: print_varset(std::bitset<32> code) {
	std::stringstream ss;

	// Get a container just for printing output correctly with commas
	std::vector<std::string> container;

	for(size_t i=0; i < 16; i++) {
		if(code[2*i]) {
			ss << data_[i];
			if(offsetMap[2*i]) ss << "(-" << offsetMap[2*i] << ")";
			ss << "<";
			container.push_back(ss.str());
			ss.str(std::string()); // Erase the stringstream
		}
		if(code[2*i+1]) {
			ss << ">"  << data_[i];
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
	for(size_t i = 0; i < size(); i++) {
		ss << data_[i]  << " -> " << i << "\n";
	}

	return ss.str();
}

void VariableFactory :: merge(VariableFactory &rhs) {
	for(auto &var: rhs.data_) {
		auto it = std::lower_bound(data_.begin(), data_.end(), var);
		if(it != data_.end() && var == *it) {
			throw parsing::BadRegex("Regex is not functional.");
		} else {
			if(size() >= MAX_VARS) {}
				// TODO: Throw an exception;
			data_.insert(it, var);

			offsetMap.push_back(0);
			offsetMap.push_back(0);
		}
	}

}

bool VariableFactory::contains(std::string var) {
	return std::binary_search(data_.begin(), data_.end(), var);
}

bool VariableFactory::empty() {
	return data_.empty();
}

bool VariableFactory::operator ==(const VariableFactory &vf) const {
	return data_ == vf.data_;
}


FilterFactory::FilterFactory(): numFilters(0) {}

std::string FilterFactory::pprint() {
	std::stringstream ss;

	for(auto &elem: codeMap) {
		ss << elem.first.label << " -> " << elem.second << "\n";
	}

	return ss.str();
}


int FilterFactory :: addFilter(CharClass cs) {
	if(!codeMap.count(cs)) {
		codeMap[cs] = numFilters;
		filterMap[numFilters] = cs;
		numFilters++;
	}
	return numFilters-1;
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
		if(isMember(it.second))
			throw parsing::BadRegex("Regex is not functional.");

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
