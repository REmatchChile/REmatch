#include <string>
#include <set>
#include <sstream>

#include "charclass.hpp"

using namespace std;

// Define static variable
const std::set<CharClass::special_code> CharClass::special_codes = {
	std::make_tuple(".", false, 1),
	std::make_tuple("\\D", true, 2),
	std::make_tuple("\\d", false, 2),
	std::make_tuple("\\W", true, 3),
	std::make_tuple("\\w", false, 3),
	std::make_tuple("\\s", false, 4),
};

// Empty constructor
// TODO: check if this constructor is used somewhere, delete it otherwise
CharClass :: CharClass(): special(0), negated(false), label("") {}

// Single char constructor
// TODO: delete this constructor and use General constructor instead
CharClass :: CharClass(const char &a): special(0), negated(false) {
	if (a == '\n')
		label = "\\n";
	else
		label = string(1, a);
	singles.insert(a);
}

// Special sets constructor
// TODO: delete this constructor and use General constructor instead
CharClass :: CharClass(int special, bool negated): special(special), negated(negated) {
	switch(special) {
		case 1:
			label = ".";
			break;
		case 2:
			if(negated) label = "\\D";
			else label = "\\d";
			break;
		case 3:
			if(negated) label = "\\W";
			else label = "\\w";
			break;
		case 4:
			label = "\\s";
			break;
	}
}

// Regex grammar charset constructor
// TODO: delete this constructor and use Charset constructor instead
CharClass :: CharClass(const ast::charset &cs) {
	special = 0;
	negated = cs.negated;
	for(auto &el: cs.elements) {
		if(el.which() == 0) {
			singles.insert(get<char>(el));
		}
		else {
			ranges.insert(get<ast::charset::range>(el));
		}
	}
	updateLabel();
}

// Charset constructor
CharClass :: CharClass(bool negated, std::set<CharClass::range> ranges, std::set<char> singles):
	special(0), negated(negated), ranges(ranges), singles(singles) { updateLabel(); }


// General constructor
CharClass :: CharClass(std::string str, bool is_special) {

	if (is_special) {
		std::string tuple_label;
		bool tuple_negated;
		int tuple_special_code;
		for (std::tuple<std::string, bool, int> const &tuple : special_codes) {
			std::tie(tuple_label, tuple_negated, tuple_special_code) = tuple;
			if (tuple_label == str) {
				label = tuple_label;
				negated = tuple_negated;
				special = tuple_special_code;
				return;
			}
		}
		throw std::invalid_argument("given string is not a special string");
	}

	if (str.length() > 1) {
		throw std::invalid_argument("non special chars must be single chars");
	}

	negated = false;
	special = 0;

	label = str.substr(0, 1);
	singles.insert(label.at(0));
}

bool CharClass :: operator==(const CharClass& rhs) const {
	return label == rhs.label; 
}

string CharClass :: print() {return label;}

void CharClass :: updateLabel() {
	set<char>::iterator it_single;
	set<tuple<char,char>>::iterator it;
	set<tuple<char,char>>::iterator it2;

	it = ranges.begin();
	char b1, b2, e1, e2;

	// Merges overlapping ranges
	while(it != ranges.end()) {
		it2 = it;
		it2++;
		while(it2 != ranges.end()) {
			b1 = get<0>(*(it)); b2 = get<0>(*(it2));
			e1 = get<1>(*(it)); e2 = get<1>(*(it2));

			if(b1 <= e2 && b2 <= e1) {  // If overlap occurs
				ranges.insert(make_tuple(min(b1, b2), max(e1, e2)));
				ranges.erase(it2);
				ranges.erase(it);
				it = ranges.begin();
				break;
			}
			it2++;
		}
		it++;
	}

	// Erases singles that are already inside (or outside if negated) of a range
	char single, b, e;
	for(auto &range: ranges) {
		for(it_single=singles.begin(); it_single != singles.end();) {
			single =  *it_single;
			b = get<0>(range); e = get<1>(range);
			if(b <= single && single <= e) {
				it_single = singles.erase(it_single);
			}	
			else {
				it_single++;
			}
		}
	}

	stringstream ss;
	string r0, r1;

	ss << "[";
	if(negated) ss << "^";

	for(auto &range: ranges) {
		r0 = get<0>(range); r1 = get<1>(range);
		if(r0 == "\n")
			r0 = "\\n";
		if(r1 == "\n")
			r1 = "\\n";
		ss << r0 << "-" << r1;
	}

	for(auto &single: singles) {
		if(single == '\n')
			ss << "\\n";
		else
			ss << single;
	}

	ss << "]";
	label = ss.str();
}

bool CharClass :: check(char a) {
	if(special) {
		switch(special) {
			case 1:
				return true;
			case 2:
				return !!isdigit(a) != negated;
			case 3:
				return !!isalnum(a) != negated;
			case 4:
				return !!isspace(a) != negated;
		}
	}
	for(auto &el: singles) {
		if(el == a) return !negated;
	}
	for(auto &el: ranges) {
		if(std::get<0>(el) <= a && a <= std::get<1>(el)) return !negated;
	}
	return negated;
}

