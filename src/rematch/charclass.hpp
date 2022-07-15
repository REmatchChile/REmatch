#ifndef CHARCLASS_HPP
#define CHARCLASS_HPP

#include <ostream>
#include <set>
#include <string>

#include <boost/functional/hash.hpp> // For boost::hash_combine

#include "parse/ast.hpp"

namespace rematch {

struct CharRange {
  CharRange() : lo(0), hi(0) {}
  CharRange(char l, char h) : lo(l), hi(h) {}

  bool operator==(const CharRange &rhs) const {
    return lo == rhs.lo && hi == rhs.hi;
  }

  char lo;
  char hi;
};

struct CharRangeLess {
  bool operator()(const CharRange &a, const CharRange &b) const {
    return a.hi < b.lo;
  }
};

using CharRangeSet = std::set<CharRange, CharRangeLess>;

class CharClass {

  friend struct std::hash<rematch::CharClass>;

public:
  CharClass();
  // Shorthand constructors
  CharClass(char c);
  CharClass(char l, char h);
  // AST structs constructors
  CharClass(ast::special const &s);
  CharClass(ast::charset const &cs);

  using iterator = CharRangeSet::iterator;
  using const_iterator = CharRangeSet::const_iterator;

  iterator begin() { return ranges_.begin(); }
  const_iterator cbegin() const { return ranges_.cbegin();}

  iterator end() { return ranges_.end(); }
  const_iterator cend() const { return ranges_.cend(); }

  int size() { return nchars_; }
  bool empty() const { return nchars_ == 0; }

  bool contains(char c);
  bool add_range(char l, char h);
  bool add_single(char c);
  void add_charclass(CharClass *cc);
  void negate();

  CharClass intersect(CharClass const &cc);
  CharClass set_minus(CharClass const &cc);

  bool do_intersect(CharClass *cc);

  bool operator==(const CharClass &rhs) const;

  friend std::ostream &operator<<(std::ostream &os, CharClass const &b);

  bool is_dot() const { return nchars_ == CHAR_MAX + 1; }

private:
  int nchars_;
  CharRangeSet ranges_;
};

} // end namespace rematch

// Hashing for CharClass
namespace std {

template <> struct hash<rematch::CharClass> {
  size_t operator()(const rematch::CharClass &ch) const {

    size_t res = 0;

    for (auto &elem : ch.ranges_) {
      boost::hash_combine(res, elem.lo);
      boost::hash_combine(res, elem.hi);
    }

    return res;
  }
};

} // end namespace std

#endif // end CHARCLASS_HPP