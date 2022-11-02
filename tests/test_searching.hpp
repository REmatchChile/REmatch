//
// Created by Nicolás Van Sint Jan on 31-10-22.
//

#ifndef REMATCH_TEST_SEARCHING_HPP
#define REMATCH_TEST_SEARCHING_HPP

#include <string>
#include <vector>

namespace rematch::testing {

struct Span {
  long long lo;
  long long hi;

  bool operator<(const Span& rhs) const {
    return std::tie(lo, hi) < std::tie(rhs.lo, rhs.hi);
  }
  bool operator>(const Span& rhs) const { return rhs < *this; }
  bool operator<=(const Span& rhs) const { return !(rhs < *this); }
  bool operator>=(const Span& rhs) const { return !(*this < rhs); }

  bool operator==(const Span& rhs) const {
    return std::tie(lo, hi) == std::tie(rhs.lo, rhs.hi);
  }
  bool operator!=(const Span& rhs) const { return !(rhs == *this); }
};

struct RegexTest {
  std::string doc;
  std::string pattern;
  std::vector<std::string> variables;
  std::vector<std::vector<Span>> mappings;
};

bool test_searching(const RegexTest& test);

}  // end namespace rematch::testing

#endif  //REMATCH_TEST_SEARCHING_HPP
