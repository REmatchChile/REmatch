//
// Created by Nicolás Van Sint Jan on 31-10-22.
//

#include "test_searching.hpp"

#include "regex/regex.hpp"

namespace rematch::testing {

bool test_searching(const RegexTest& test) {
  auto expected_results = test.mappings;

  auto rgx{test.pattern};
  auto miter = rgx.find_iter(test.doc);
}

}  // namespace rematch::testing