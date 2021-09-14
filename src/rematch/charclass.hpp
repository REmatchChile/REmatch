#ifndef CHARCLASS_HPP
#define CHARCLASS_HPP

#include <set>
#include <string>
#include <ostream>

#include <boost/functional/hash.hpp> // For boost::hash_combine

#include "parse/ast.hpp"

namespace rematch {

struct CharRange {
	CharRange() : lo(0), hi(0) {}
	CharRange(char l, char h): lo(l), hi(h) {}

	bool operator==(const CharRange& rhs) const { return lo == rhs.lo && hi == rhs.hi; }

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

 friend struct std::hash<rematch::CharClassBuilder>;

 public:
	CharClassBuilder();
	// Shorthand constructors
	CharClassBuilder(char c);
	CharClassBuilder(char l, char h);
	// AST structs constructors
	CharClassBuilder(ast::special const &s);
	CharClassBuilder(ast::charset const &cs);

	using iterator = CharRangeSet::iterator;
	iterator begin() {return ranges_.begin();}
	iterator end() {return ranges_.end();}

	int size() {return nchars_;}
	bool empty() const {return nchars_ == 0;}

	bool contains(char c);
	bool add_range(char l, char h);
	bool add_single(char c);
	void add_charclass(CharClassBuilder* cc);
	void negate();

	std::unique_ptr<CharClass> get_charclass();

	CharClassBuilder* intersect(CharClassBuilder* cc);
	CharClassBuilder* set_minus(CharClassBuilder* cc);


	bool operator==(const CharClassBuilder& rhs) const;

	friend std::ostream& operator<<(std::ostream &os, CharClassBuilder const &b);

	bool is_dot() const { return nchars_ == CHAR_MAX+1;}

 private:
	int nchars_;
	CharRangeSet ranges_;
};


class CharClass {
 public:
	// Check if CharClass contains a char
	bool contains(char a);

	CharClass(int maxranges) : ranges_(new CharRange[maxranges]) {}

	~CharClass();

	bool is_dot() const {return nranges_ == 1 && ranges_[0].lo == 0 && ranges_[0].hi == CHAR_MAX;}

	friend std::ostream& operator<<(std::ostream &os, CharClass const &cc);
 private:

	friend class CharClassBuilder;
	friend class FilterFactory;
	friend struct std::hash<rematch::CharClass>;

	int nchars_;
	CharRange* ranges_;
	int nranges_;

	CharClass(const CharClass&) = default;
	CharClass& operator=(const CharClass&) = default;
	bool operator<(const CharClass& cc) const;

}; // end class CharClass

} // end namespace rematch


// Hashing for the class
namespace std {

template <>
struct hash<rematch::CharClassBuilder> {
	size_t operator()(const rematch::CharClassBuilder& ch) const {

		size_t res = 0;

		for(auto &elem: ch.ranges_) {
			boost::hash_combine(res, elem.lo);
			boost::hash_combine(res, elem.hi);
		}

		return res;
	}
};

} // end namespace std

#endif // end CHARCLASS_HPP