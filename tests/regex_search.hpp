#ifndef TESTS__TEST_CXXTEST
#define TESTS__TEST_CXXTEST

#include <cxxtest/TestSuite.h>

#include "regex/regex.hpp"

struct Span {
  int lo;
  int hi;
};

struct RegexTest {
  std::string doc;
  std::string pattern;
  std::vector<std::string> variables;
  std::vector<std::vector<Span>> mappings;
};

class TestRegexResults : public CxxTest::TestSuite {
public:
  void test_all_substrings() {
    TS_ASSERT_EQUALS(1, 1);
    TS_ASSERT_EQUALS(1, 2);
  }

  void test_addition() {
    TS_ASSERT_EQUALS(1 + 1, 2);
    TS_ASSERT_EQUALS(2 + 2, 5);
  }
};

#endif // TESTS__TEST_CXXTEST