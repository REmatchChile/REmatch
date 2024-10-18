#include "parsing/charclass.hpp"

namespace REmatch{
inline namespace parsing {

CharClass::CharClass(): nchars(0) {}

CharClass::CharClass(char c): nchars(0) { add_single(c); }

CharClass::CharClass(char l, char h): nchars(0) { add_range(l,h); }

bool CharClass::add_range(char lo, char hi) {
	if (hi < lo)
		return false;

	{
		auto it = ranges.find(CharRange(lo, hi));
		if (it != end() && it->lo <= lo && hi <= it->hi)
			return false;
	}

	// Look for an inmediate range to the left of [lo, hi]. If exists
	// then erase and extend [lo, hi] accordingly
	if(lo > 0) {
		auto it = ranges.find(CharRange(lo-1, lo-1));
		if(it != end()) {
			lo = it->lo;
			if(it->hi > hi)
				hi = it->hi;
			nchars -= it->hi - it->lo + 1;
			ranges.erase(it);
		}
	}
	// Look for an inmediate range to the right of [lo, hi]. If exists
	// then erase and extend [lo, hi] accordingly
	if(hi < CHAR_MAX) {
		auto it = ranges.find(CharRange(hi+1, hi+1));
		if(it != end()) {
			hi = it->hi;
			// Not necesary to check for lo. Did it in previous step.
			nchars -= it->hi - it->lo + 1;
			ranges.erase(it);
		}
	}

	// Search for ranges inside [lo, hi]. Erase them.
	// The remaining ranges that overlap with [lo, hi] must be inside
	// it.
	for(;;) {
		auto it = ranges.find(CharRange(lo, hi));
		if(it == end())
			break;
		nchars -= it->hi - it->lo + 1;
		ranges.erase(it);
	}

	// Add [lo, hi]
	nchars += hi - lo + 1;
	ranges.insert(CharRange(lo, hi));
	return true;
}

bool CharClass::add_single(char c) {
	return add_range(c, c);
}

void CharClass::add_charclass(CharClass *cc) {
	for(iterator it = cc->begin(); it != cc->end(); it++) {
		add_range(it->lo, it->hi);
	}
}

bool CharClass::contains(char c) {
	return ranges.find(CharRange(c, c)) != end();
}

void CharClass::negate() {
	std::vector<CharRange> v;
	v.reserve(ranges.size() + 1);

	iterator it = begin();
	if(it == end()) {
		v.push_back(CharRange(0, CHAR_MAX));
	} else {
		int next_lo = 0;
		if(it->lo == 0) {
			next_lo = it->hi + 1;
			it++;
		}
		for(; it != end(); it++) {
			v.push_back(CharRange(next_lo, it->lo-1));
			next_lo = it->hi + 1;
		}
		if(next_lo <= CHAR_MAX) {
			v.push_back(CharRange(next_lo, CHAR_MAX));
		}
	}

	ranges.clear();
	for(auto chrange: v) {
		ranges.insert(chrange);
	}

	nchars = CHAR_MAX+1 - nchars;
}

CharClass* CharClass::intersect(CharClass *cc) {
	CharClass* new_cc = new CharClass();
	for(iterator it1 = cc->begin(); it1 != cc->end(); it1++) {
		iterator it2 = ranges.lower_bound(*it1); // lowest range that overlaps
		if (it2 != end()) {
			for(; it2 != ranges.upper_bound(*it1); it2++) {
				char lo = std::max(it2->lo, it1->lo);
				char hi = std::min(it2->hi, it1->hi);
				new_cc->add_range(lo, hi);
			}
		}
	}
	return new_cc;
}

CharClass* CharClass::set_minus(CharClass *cc) {
	// Set minus is just the intersection with the complement;
	cc->negate();

	CharClass* new_cc = intersect(cc);

	cc->negate();

	return new_cc;
}

// std::ostream& operator<<(std::ostream &os, CharClass const &b) {
// 	for(auto &range: b.ranges) {
// 		os << '[' << (int)range.lo << ',' << (int)range.hi << ']';
// 	}
// 	return os;
// }

bool CharClass::operator==(const CharClass& rhs) const {
	return ranges == rhs.ranges;
}


std::ostream& operator<<(std::ostream &os, CharClass const &cc) {
	if(cc.nchars == 1) return os << std::bitset<8>(cc.ranges.begin()->lo);
	os << "[";
	for(auto &range: cc.ranges) {
		if(range.lo == range.hi)
			os << '(' << std::bitset<8>(range.lo) << ')';
		else
			os << '(' << std::bitset<8>(range.lo) << '-' << std::bitset<8>(range.hi) << ')';
	}
	os << "]";
	return os;
}


}
}
