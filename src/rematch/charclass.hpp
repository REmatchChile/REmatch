#ifndef CHARCLASS_HPP
#define CHARCLASS_HPP

#include <set>
#include <string>
#include <tuple>

#include "parse/regex/ast.hpp"

namespace rematch {

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