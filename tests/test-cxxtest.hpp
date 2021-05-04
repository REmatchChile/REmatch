#ifndef TESTS__TEST_CXXTEST
#define TESTS__TEST_CXXTEST

#include <cxxtest/TestSuite.h>

#include "regex/regex.hpp"

class SimpleTest : public CxxTest::TestSuite {
 public:
  void test_equality() {
    TS_ASSERT_EQUALS(1, 1);
    TS_ASSERT_EQUALS(1, 2);
  }

  void test_addition() {
    TS_ASSERT_EQUALS(1 + 1, 2);
    TS_ASSERT_EQUALS(2 + 2, 5);
  }
};

#endif // TESTS__TEST_CXXTEST