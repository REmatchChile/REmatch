#ifndef CHARCLASS_BUILDER_HPP
#define CHARCLASS_BUILDER_HPP

#include "parsing/charclass.hpp"

namespace rematch {

class CharClassBuilder {

 friend struct std::hash<rematch::CharClassBuilder>;

 private:
	int nchars_;
	CharRangeSet ranges_;

 public:
	CharClassBuilder();
	// Shorthand constructors
	CharClassBuilder(char c);
	CharClassBuilder(char l, char h);

	using iterator = CharRangeSet::iterator;
	iterator begin() {return ranges_.begin();}
	iterator end() {return ranges_.end();}

	std::unique_ptr<CharClass> get_charclass();
	void add_charclass(CharClassBuilder* cc);
	bool add_range(char l, char h);
	bool add_single(char c);
	void negate();

	CharClassBuilder* intersect(CharClassBuilder* cc);
	CharClassBuilder* set_minus(CharClassBuilder* cc);

	int size() {return nchars_;}
	bool empty() const {return nchars_ == 0;}
	bool contains(char c);
	bool is_dot() const { return nchars_ == CHAR_MAX+1;}
	bool operator==(const CharClassBuilder& rhs) const;

	friend std::ostream& operator<<(std::ostream &os, CharClassBuilder const &b);
};

}

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

#endif // end charclass_hpp
