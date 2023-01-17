#include <iostream>
#include <unordered_map>
#include <vector>
#include <ostream>
#include <bitset>
#include <unordered_set>
#include <algorithm>

#include "parsing/factories/filter_factory.hpp"
#include "parsing/charclass.hpp"
#include "parsing/bad_regex_exception.hpp"

namespace rematch {

FilterFactory::FilterFactory() {
	CharClassBuilder ccb;
	ccb.add_range('\0', '\0');
	add_filter(ccb);
}

std::string FilterFactory::pprint() {
	std::stringstream ss;

	for(auto &elem: code_map_) {
		ss << elem.first << " -> " << elem.second << "\n";
	}

	return ss.str();
}


int FilterFactory::add_filter(CharClassBuilder ccb) {
	auto res = code_map_.insert({ccb, size_});
	if(res.second) {
		filter_map_.insert({size_, ccb});
		return size_++;
	} else {
		return res.first->second;
	}
}

int FilterFactory::get_code(CharClassBuilder cs) {
	return code_map_.at(cs);
}

CharClassBuilder& FilterFactory :: get_filter(int code) {
	return filter_map_[code];
}

bool FilterFactory::contains(CharClassBuilder &cs) const {
	return code_map_.find(cs) != code_map_.end();
}

void FilterFactory::merge(FilterFactory &rest) {
	/**
	* Merges the FilterFactory with another one inplace. Used at parsing
	* post-processing.
	*/

	for(auto &it : rest.filter_map_) {
		if(contains(it.second))
			throw BadRegexException("Regex is not functional.");

		filter_map_[size_] = it.second;
		code_map_[it.second] = size_;
		size_++;
	}
}

std::unordered_map<std::vector<bool>, std::vector<char>>  FilterFactory::allPossibleCharBitsets() {
	std::unordered_map<std::vector<bool>, std::vector<char>> ret;
	for(char a = (char)32; a < 127; a++) {
    std::vector<bool> bs = applyFilters(a);
    auto it = ret.find(bs);
    if(it == ret.end())
      ret.insert(std::make_pair(bs, std::vector<char>(1, a)));
    else
      it->second.push_back(a);
	}
	return ret;
}


std::vector<bool> FilterFactory::applyFilters(char a) {
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
		std::vector<bool> bitsetVector(size_);
		for(auto &it: filter_map_) {
			bitsetVector[it.first] =  it.second.contains(a);
		}
		bitsetMap.insert(std::make_pair(a, bitsetVector));

		return bitsetVector;
	}


	return it->second;

}

} // end namespace rematch
