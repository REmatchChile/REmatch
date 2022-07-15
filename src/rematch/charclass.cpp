#include "charclass.hpp"

#include <algorithm>
#include <ostream>
#include <set>
#include <string>
#include <vector>

namespace rematch {

CharClass::CharClass() : nchars_(0) {}

CharClass::CharClass(char c) : nchars_(0) { add_single(c); }

CharClass::CharClass(char l, char h) : nchars_(0) {
  add_range(l, h);
}

CharClass::CharClass(ast::special const &s) : nchars_(0) {
  switch (s.code_) {
  case SpecialCode::kAnyChar:
    add_range(0, CHAR_MAX);
    break;
  case SpecialCode::kAnyDigit:
    add_range('0', '9');
    break;
  case SpecialCode::kAnyWhiteSpace:
    add_range(9, 13); // Whitespaces: [\n\f\r\t]
    add_single(32);   // Space: ' '
    break;
  case SpecialCode::kAnyWord:
    add_range('A', 'Z');
    add_range('a', 'z');
    add_range('0', '9');
    break;
  default:
    break;
  }

  if (!s.not_negated_)
    negate();
}

CharClass::CharClass(ast::charset const &cs) : nchars_(0) {
  for (auto &elem : cs.elements) {
    if (elem.which() == 0) {
      add_single(boost::get<char>(elem));
    } else {
      std::tuple<char, char> tup = boost::get<ast::charset::range>(elem);
      add_range(std::get<0>(tup), std::get<1>(tup));
    }
  }
  // If it is negated we have to negate the ccb
  if (cs.negated)
    negate();
}

bool CharClass::add_range(char lo, char hi) {
  if (hi < lo)
    return false;

  {
    auto it = ranges_.find(CharRange(lo, hi));
    if (it != end() && it->lo <= lo && hi <= it->hi)
      return false;
  }

  // Look for an inmediate range to the left of [lo, hi]. If exists
  // then erase and extend [lo, hi] accordingly
  if (lo > 0) {
    auto it = ranges_.find(CharRange(lo - 1, lo - 1));
    if (it != end()) {
      lo = it->lo;
      if (it->hi > hi)
        hi = it->hi;
      nchars_ -= it->hi - it->lo + 1;
      ranges_.erase(it);
    }
  }
  // Look for an inmediate range to the right of [lo, hi]. If exists
  // then erase and extend [lo, hi] accordingly
  if (hi < CHAR_MAX) {
    auto it = ranges_.find(CharRange(hi + 1, hi + 1));
    if (it != end()) {
      hi = it->hi;
      // Not necesary to check for lo. Did it in previous step.
      nchars_ -= it->hi - it->lo + 1;
      ranges_.erase(it);
    }
  }

  // Search for ranges inside [lo, hi]. Erase them.
  // The remaining ranges that overlap with [lo, hi] must be inside
  // it.
  for (;;) {
    auto it = ranges_.find(CharRange(lo, hi));
    if (it == end())
      break;
    nchars_ -= it->hi - it->lo + 1;
    ranges_.erase(it);
  }

  // Add [lo, hi]
  nchars_ += hi - lo + 1;
  ranges_.insert(CharRange(lo, hi));
  return true;
}

bool CharClass::add_single(char c) { return add_range(c, c); }

void CharClass::add_charclass(CharClass *cc) {
  for (iterator it = cc->begin(); it != cc->end(); it++) {
    add_range(it->lo, it->hi);
  }
}

bool CharClass::contains(char c) {
  return ranges_.find(CharRange(c, c)) != end();
}

void CharClass::negate() {
  std::vector<CharRange> v;
  v.reserve(ranges_.size() + 1);

  iterator it = begin();
  if (it == end()) {
    v.push_back(CharRange(0, CHAR_MAX));
  } else {
    int next_lo = 0;
    if (it->lo == 0) {
      next_lo = it->hi + 1;
      it++;
    }
    for (; it != end(); it++) {
      v.push_back(CharRange(next_lo, it->lo - 1));
      next_lo = it->hi + 1;
    }
    if (next_lo <= CHAR_MAX) {
      v.push_back(CharRange(next_lo, CHAR_MAX));
    }
  }

  ranges_.clear();
  for (auto chrange : v) {
    ranges_.insert(chrange);
  }

  nchars_ = CHAR_MAX + 1 - nchars_;
}

CharClass CharClass::intersect(CharClass const &cc) {
  CharClass new_cc;
  for (iterator it1 = cc.cbegin(); it1 != cc.cend(); it1++) {
    iterator it2 = ranges_.lower_bound(*it1); // lowest range that overlaps
    if (it2 != end()) {
      for (; it2 != ranges_.upper_bound(*it1); it2++) {
        char lo = std::max(it2->lo, it1->lo);
        char hi = std::min(it2->hi, it1->hi);
        new_cc.add_range(lo, hi);
      }
    }
  }
  return new_cc;
}

bool CharClass::do_intersect(CharClass *cc) {
  for (iterator it1 = cc->begin(); it1 != cc->end(); it1++) {
    iterator it2 = ranges_.lower_bound(*it1); // lowest range that overlaps
    if (it2 != end())
      return true;
  }
  return false;
}

CharClass CharClass::set_minus(CharClass const &cc) {
  // Set minus is just the intersection with the complement;
  CharClass copy_cc(cc);
  copy_cc.negate();

  CharClass new_cc = intersect(copy_cc);

  return new_cc;
}

bool CharClass::operator==(const CharClass &rhs) const {
  return ranges_ == rhs.ranges_;
}

std::ostream &operator<<(std::ostream &os, CharClass const &cc) {
  if (cc.is_dot())
    return os << '.';
  else if (cc.nchars_ == 1)
    return os << (char)cc.ranges_.begin()->lo;
  os << "[";
  for (auto &range : cc.ranges_) {
    if (range.lo == range.hi)
      os << (char)range.lo;
    else
      os << (char)range.lo << '-' << (char)range.hi;
  }
  os << "]";
  return os;
}

} // end namespace rematch
