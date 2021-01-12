#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#define DYNAMIC_BITSET true

#include <bitset>
#include <cassert>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <boost/dynamic_bitset.hpp>

#include "bitsetwrapper.hpp"

extended_bitset :: extended_bitset(size_t size) {
	bitsetSize = size;
	for(size_t i=0; i < size / 64 + 1; i++) {
		bitsetArray.emplace_back();
	}
}

void extended_bitset :: set(size_t pos, bool val) {
	size_t index, offset;
	index = pos / 64;
	offset = pos % 64;
	bitsetArray[index].set(offset, val);
}

bool extended_bitset :: get(size_t pos) {
	size_t index, offset;
	index = pos / 64;
	offset = pos % 64;
	return bitsetArray[index][offset];
}

size_t extended_bitset :: count() const {
	size_t ctr = 0;
	for(auto &bitset: bitsetArray) {
		ctr += bitset.count();
	}

	return ctr;
}

size_t extended_bitset :: size() const {
	return bitsetSize;
}

bool extended_bitset :: operator==(const extended_bitset &rhs) const {
	if(bitsetSize != rhs.bitsetSize) return false;
	for(size_t i=0; i < bitsetArray.size(); i++) {
		if(bitsetArray[i] != rhs.bitsetArray[i]) return false;
	}
return true;
}

extended_bitset& extended_bitset :: operator|=(const extended_bitset &rhs) {
	size_t m = std::min(bitsetArray.size(), rhs.bitsetArray.size());
	for(size_t i=0; i<m; i++) {
		bitsetArray[i] |= rhs.bitsetArray[i];
	}

	return *this;
}


BitsetWrapper :: BitsetWrapper(size_t size) {
	bitsetSize = size;
	if(DYNAMIC_BITSET) {
		dyn = new boost::dynamic_bitset<>(size);
	}
	else {
		ext = new extended_bitset(size);
	}
}

void BitsetWrapper :: set(size_t pos, bool val) {
	if(DYNAMIC_BITSET) {
		(*dyn)[pos] = val;
	}
	else {
		ext->set(pos, val);
	}
}

void BitsetWrapper :: clear() {
	if(DYNAMIC_BITSET) {
		(*dyn).clear();
	}
}

bool BitsetWrapper :: any() const {
	if(DYNAMIC_BITSET) {
		return (*dyn).any();
	}
	return false;
}



void BitsetWrapper :: resize(size_t num_bits) {
	if(DYNAMIC_BITSET) {
		(*dyn).resize(num_bits);
	}
}


bool BitsetWrapper :: get(size_t pos) const{
	if(DYNAMIC_BITSET) {
		return (*dyn)[pos];
	}
	else {
		return ext->get(pos);
	}
}

size_t BitsetWrapper :: count() const {
	if(DYNAMIC_BITSET) {
		return dyn->count();
	}
	else {
		return ext->count();
	}
}

size_t BitsetWrapper :: size() const {
	return bitsetSize;
}

/****** OPERATORS ******/

BitsetWrapper& BitsetWrapper :: operator|=(const BitsetWrapper &rhs) {
	if(DYNAMIC_BITSET) {
		*dyn |= *rhs.dyn;		
	}
	else {
		*ext |= *rhs.ext;
	}

	return *this;
}

bool BitsetWrapper :: operator==(const BitsetWrapper &rhs) const {
	/* Overload for use in hashing tables */
	if(DYNAMIC_BITSET) {
		return (*dyn) == (*rhs.dyn);
	}
	return (*ext) == (*rhs.ext);
}


boost::dynamic_bitset<>* BitsetWrapper :: getDynamic() const {
	return dyn;
}
extended_bitset* BitsetWrapper :: getExtended() const {
	return ext;
}


std::ostream& operator<<(std::ostream &os, extended_bitset const &bs) {
	/* Overload for printing to cout*/
	std::stringstream ss;

	for(auto &bitset: bs.bitsetArray) {
		ss << bitset;
	}

	return os << ss.str();
}

std::ostream& operator<<(std::ostream &os, BitsetWrapper const &bs) {
	/* Overload for printing to cout*/
	if(DYNAMIC_BITSET) {
		return os << *(bs.dyn);
	}
	return os << *(bs.ext);
}


