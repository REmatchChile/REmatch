#include <string>
#include <set>
#include <sstream>

#include "charclass.hpp"

namespace rematch {

// Define static variable
const std::set<CharClass::special_code> CharClass::special_codes = {
	std::make_tuple(".", 		false,	SpecialCode::kAnyChar),
	std::make_tuple("\\D", 	true,		SpecialCode::kAnyDigit),
	std::make_tuple("\\d", 	false,	SpecialCode::kAnyDigit),
	std::make_tuple("\\W", 	true, 	SpecialCode::kAnyWord),
	std::make_tuple("\\w", 	false, 	SpecialCode::kAnyWord),
	std::make_tuple("\\s", 	false, 	SpecialCode::kAnyWhiteSpace),
	std::make_tuple("\\S", 	true, 	SpecialCode::kAnyWhiteSpace)
};

// Empty constructor
CharClass::CharClass()
		: label(""),
			special_(SpecialCode::kNoSpecial),
			negated_(false) {}

// Single char constructor
CharClass::CharClass(const char &a)
		: special_(SpecialCode::kNoSpecial),
			negated_(false) {
// FIXME: This is incomplete/badly implemented
	if (a == '\n')
		label = "\\n";
	else
		label = std::string(1, a);
	singles.insert(a);
}

// Special sets constructor
CharClass::CharClass(const ast::special &s)
		: special_(s.code_),
			negated_(!s.not_negated_) {
	switch(special_) {
		case SpecialCode::kAnyChar:
			label = ".";
			break;
		case SpecialCode::kAnyDigit:
			if(negated_)
				label = "\\D";
			else
				label = "\\d";
			break;
		case SpecialCode::kAnyWord:
			if(negated_)
				label = "\\W";
			else
				label = "\\w";
			break;
		case SpecialCode::kAnyWhiteSpace:
			if(negated_)
				label = "\\S";
			else
				label = "\\s";
			break;
		default:
			break;
	}
}

// Regex grammar charset constructor
CharClass::CharClass(const ast::charset &cs)
		:	special_(SpecialCode::kNoSpecial),
			negated_(cs.negated) {
	for(auto &el: cs.elements) {
		if(el.which() == 0) {
			singles.insert(boost::get<char>(el));
		}
		else {
			ranges.insert(boost::get<ast::charset::range>(el));
		}
	}
	updateLabel();
}

// Charset constructor
CharClass::CharClass(bool negated, std::set<CharClass::range> ranges, std::set<char> singles)
		:	special_(SpecialCode::kNoSpecial),
			negated_(negated),
			ranges(ranges),
			singles(singles) {
	updateLabel();
}


// General constructor
CharClass::CharClass(std::string str, bool is_special) {
	if (is_special) {
		std::string tuple_label;
		bool tuple_negated;
		SpecialCode tuple_special_code;
		for (auto const &tuple : special_codes) {
			std::tie(tuple_label, tuple_negated, tuple_special_code) = tuple;
			if (tuple_label == str) {
				label = tuple_label;
				negated_ = tuple_negated;
				special_ = tuple_special_code;
				return;
			}
		}
		throw std::invalid_argument("given string is not a special string");
	}

	if (str.length() > 1) {
		throw std::invalid_argument("non special chars must be single chars");
	}

	negated_ = false;
	special_ = SpecialCode::kNoSpecial;

	label = str.substr(0, 1);
	singles.insert(label.at(0));
}

bool CharClass :: operator==(const CharClass& rhs) const {
	return label == rhs.label;
}

std::string CharClass :: print() {return label;}

void CharClass :: updateLabel() {
	std::set<char>::iterator it_single;
	std::set<std::tuple<char,char>>::iterator it;
	std::set<std::tuple<char,char>>::iterator it2;

	it = ranges.begin();
	char b1, b2, e1, e2;

	// Merges overlapping ranges
	while(it != ranges.end()) {
		it2 = it;
		it2++;
		while(it2 != ranges.end()) {
			b1 = std::get<0>(*(it)); b2 = std::get<0>(*(it2));
			e1 = std::get<1>(*(it)); e2 = std::get<1>(*(it2));

			if(b1 <= e2 && b2 <= e1) {  // If overlap occurs
				ranges.insert(std::make_tuple(std::min(b1, b2), std::max(e1, e2)));
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
			b = std::get<0>(range); e = std::get<1>(range);
			if(b <= single && single <= e) {
				it_single = singles.erase(it_single);
			}
			else {
				it_single++;
			}
		}
	}

	std::stringstream ss;
	std::string r0, r1;

	ss << "[";
	if(negated_) ss << "^";

	for(auto &range: ranges) {
		r0 = std::get<0>(range); r1 = std::get<1>(range);
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
	switch(special_) {
		case SpecialCode::kNoSpecial:
			for(auto &el: singles) {
				if(el == a) return !negated_;
			}
			for(auto &el: ranges) {
				if(std::get<0>(el) <= a && a <= std::get<1>(el)) return !negated_;
			}
			return negated_;
		case SpecialCode::kAnyChar:
			return a != 0x0; // Null char reserved
		case SpecialCode::kAnyDigit:
			return !!isdigit(a) != negated_;
		case SpecialCode::kAnyWord:
			return !!isalnum(a) != negated_;
		case SpecialCode::kAnyWhiteSpace:
			return !!isspace(a) != negated_;
	}
}

} // end namespace rematch
