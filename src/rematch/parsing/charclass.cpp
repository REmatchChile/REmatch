#include "parsing/charclass.hpp"

#include <string>
#include <set>
#include <ostream>
#include <algorithm>
#include <vector>

namespace rematch {

CharClass::~CharClass() {
	delete [] ranges_;
}

bool CharClass::contains(char a) {
	CharRange* range = ranges_;
	int n = nranges_;

	while(n > 0) {
		int m = n/2;
		if(range[m].hi < a) {
			range += m+1;
			n -= m+1;
		} else if (a < range[m].lo) {
			n = m;
		} else { // range[m].lo <= a && a <= range[m].hi
			return true;
		}
	}
	return false;
}

bool CharClass::operator<(const CharClass& cc) const {
	for(int i=0; i < std::min(nranges_, cc.nranges_); i++) {
		if(ranges_[i].lo < cc.ranges_[i].lo) return true;
		else if(ranges_[i].lo > cc.ranges_[i].lo) return false;
		else if(ranges_[i].hi < cc.ranges_[i].hi) return true;
		else if(ranges_[i].hi > cc.ranges_[i].hi) return false;
	}

	return nranges_ < cc.nranges_;
}


} // end namespace rematch
