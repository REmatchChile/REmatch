#ifndef BITSETWRAPPER_HPP
#define BITSETWRAPPER_HPP

#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#define DYNAMIC_BITSET true

#include <bitset>
#include <cassert>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <boost/dynamic_bitset.hpp>

using DynamicBitset = boost::dynamic_bitset<>;


template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
/* Combine hashes inspired in boost::hash_combine*/
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


struct extended_bitset {
	std::vector<std::bitset<64>> bitsetArray;
	size_t bitsetSize;

	extended_bitset(size_t size);

	void set(size_t pos, bool val);
	bool get(size_t pos);
	size_t count() const;
	size_t size() const;

	bool operator==(const extended_bitset &rhs) const;
	extended_bitset& operator|=(const extended_bitset &rhs);

	friend std::ostream& operator<<(std::ostream &os, extended_bitset const &bs);

};


class BitsetWrapper {
private:
	extended_bitset* ext;
	boost::dynamic_bitset<>* dyn;
	size_t bitsetSize;

public:

	BitsetWrapper(size_t size);
	void set(size_t pos, bool val);
	bool get(size_t pos) const;
	size_t count() const;
	size_t size() const;
	void clear();
	void resize(size_t num_bits);
	bool any() const;

	/****** OPERATORS ******/

	BitsetWrapper &operator|=(const BitsetWrapper &rhs);

	bool operator==(const BitsetWrapper &rhs) const;

	friend std::ostream & operator<<(std::ostream &os, BitsetWrapper const &bs);

	boost::dynamic_bitset<> *getDynamic() const;
	extended_bitset *getExtended() const;

};


std::ostream& operator<<(std::ostream &os, extended_bitset const &bs);

std::ostream& operator<<(std::ostream &os, BitsetWrapper const &bs);


namespace std {
	template <>
	struct hash<boost::dynamic_bitset<>> {
		size_t operator()(const boost::dynamic_bitset<>& bitset) const {
			size_t res = 0;
			for (auto s : bitset.m_bits)
				hash_combine(res, s);
			return res;
			}
		};

	template <>
	struct hash<extended_bitset> {
		size_t operator()(const extended_bitset& bitset) const {
			size_t res = 0;
			for (auto s : bitset.bitsetArray)
				hash_combine(res, s);
			return res;
			}
		};

	template <>
	struct hash<BitsetWrapper> {
		size_t operator()(const BitsetWrapper& bitset) const {
			size_t res = 0;
			if(DYNAMIC_BITSET) {
				for (auto s : bitset.getDynamic()->m_bits)
					hash_combine(res, s);
				return res;
			} else {
				for (auto s : bitset.getExtended()->bitsetArray)
					hash_combine(res, s);
				return res;
			}

		}
	};
}

#endif
