#ifndef CHARCLASS_HPP
#define CHARCLASS_HPP

#include <set>
#include <string>
#include <tuple>

#include "parser/ast.hpp"

// Enum for special character classes (e.g. NONDIGIT = [^0-9])
enum special {ANYCHAR=1, ANYDIGIT=2, ANYWORD=3, ANYSPACE=4};

// TODO: Inherit special charclasses

class CharClass {
	/* Extension of ast::charset (parsing struct) that stores the information of a 
	   regex charclass (e.g [^a-zA-Z0-9]) */

	public:
		using range = std::tuple<char,char>;
		using special_code = std::tuple<std::string, bool, int>;

		static const std::set<special_code> special_codes;

		int special;
		bool negated; 
		std::string label;
		std::set<range> ranges;
		std::set<char> singles;


		// Empty constructor
		// TODO: check if this constructor is used somewhere, delete it otherwise
		CharClass();

		// Single char constructor
		// TODO: delete this constructor and use General constructor instead
		CharClass(const char &a);

		// Special sets constructor
		// TODO: delete this constructor and use General constructor instead
		CharClass(int special, bool negated);

		// Regex grammar charset constructor
		// TODO: delete this constructor and use Charset constructor instead
		CharClass(const ast::charset &cs);

		// General costructor
		CharClass(std::string label, bool is_special);

		// Charset constructor
		CharClass(bool negated, std::set<range> ranges, std::set<char> singles);


	bool operator==(const CharClass& rhs) const;

	std::string print();

	void updateLabel();

	bool check(char a);
};


// Hashing for the class 
namespace std {
template <>
	struct hash<CharClass> {
		size_t operator()(const CharClass& ch) const {
			hash<string> hasher;
			return hasher(ch.label);
			}
		};
}

#endif