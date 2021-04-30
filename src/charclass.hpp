#ifndef CHARCLASS_HPP
#define CHARCLASS_HPP

#include <set>
#include <string>
#include <tuple>

#include "parse/regex/ast.hpp"

namespace rematch {

struct CharRange {
	CharRange() : lo(0), hi(0) {}
	CharRange(char l, char h): lo(l), hi(h) {}

	char lo;
	char hi;
};

struct CharRangeLess {
	bool operator()(const CharRange &a, const CharRange &b) const {
		return a.hi < b.lo;
	}
};

using CharRangeSet = std::set<CharRange, CharRangeLess>;

class CharClass;

class CharClassBuilder {
 public:
	CharClassBuilder();

	using iterator = CharRangeSet::iterator;
	iterator begin() {return ranges_.begin();}
	iterator end() {return ranges_.end();}

	int size() {return nchars_;}
	bool empty() const {return nchars_ == 0;}

	bool contains(char c);
	bool add_range(char l, char h);
	void add_charclass(CharClassBuilder* cc);
	void negate();

	CharClass* get_charclass();

	CharClassBuilder* intersect(CharClassBuilder* cc);
	CharClassBuilder* set_minus(CharClassBuilder* cc);

	friend std::ostream& operator<<(std::ostream &os, CharClassBuilder const &b);

 private:
	int nchars_;
	CharRangeSet ranges_;
};

class CharClass {
	/* Extension of ast::charset (parsing struct) that stores the information of a
	   regex charclass (e.g [^a-zA-Z0-9]) */

 public:
	using range = std::tuple<char,char>;
	using special_code = std::tuple<std::string, bool, SpecialCode>;

	static const std::set<special_code> special_codes;

	std::string label;

	// Empty constructor
	CharClass();

	// Single char constructor
	CharClass(const char &a);

	// Special sets constructor
	CharClass(const ast::special &s);

	// Regex grammar charset constructor
	CharClass(const ast::charset &cs);

	// General costructor
	CharClass(std::string label, bool is_special);

	// Charset constructor
	CharClass(bool negated, std::set<range> ranges, std::set<char> singles);

	bool operator==(const CharClass& rhs) const;

	std::string print();

	// Updates the label_ member accordingly to ranges_ and singles_. The idea
	// is to make it a canonical representation.
	void updateLabel();

	// Checks if char is inside the CharClass
	bool check(char a);

 private:
	SpecialCode special_;
	bool negated_;
	std::set<range> ranges;
	std::set<char> singles;
};

} // end namespace rematch


// Hashing for the class
namespace std {

template <>
struct hash<rematch::CharClass> {
	size_t operator()(const rematch::CharClass& ch) const {
		hash<string> hasher;
		return hasher(ch.label);
	}
};

} // end namespace std

#endif // end CHARCLASS_HPP