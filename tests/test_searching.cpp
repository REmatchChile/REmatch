//
// Created by Nicolás Van Sint Jan on 31-10-22.
//

#include "test_searching.hpp"

#include <catch2/catch_test_macros.hpp>

#include "regex/regex.hpp"
#include "data/search.cpp"

namespace rematch::testing {

struct RegexSubTest {
  std::string doc;
  std::vector<std::vector<Span>> maps;
};

bool test_searching(const RegexTest& test, RegExOptions opts) {
  auto expected = test.mappings;

  std::sort(expected.begin(), expected.end());

  RegEx rgx{test.pattern, opts};

  INFO("Pattern to search: " << test.pattern);
  INFO("Document to match: \"" << test.doc << "\"");

  REQUIRE(std::is_permutation(rgx.variables.begin(), rgx.variables.end(), test.variables.begin()));

  MatchIterator miter = rgx.find_iter(test.doc);

  std::vector<std::vector<Span>> results;

  for(auto match = miter.next(); match != nullptr; match = miter.next()) {
    std::vector<Span> row;
    for (const auto& var : test.variables) {
      row.push_back({match->start(var), match->end(var)});
    }
    results.push_back(row);
  }

  std::sort(results.begin(), results.end());

  std::vector<std::vector<Span>> additional, missing;

  std::set_difference(results.begin(), results.end(), expected.begin(),
                      expected.end(), std::inserter(additional, additional.begin()));

  std::set_difference(expected.begin(), expected.end(), results.begin(),
                      results.end(), std::inserter(missing, missing.begin()));

  std::stringstream ss1, ss2;
  if(!additional.empty()) {
    for(const auto &elem: additional) {
      for (size_t i = 0; i < rgx.variables.size() - 1; ++i) {
        ss1 << rgx.variables[i] << "|" << elem[i].lo << "," << elem[i].hi << ">, ";
      }
      ss1 << rgx.variables.back() << "|" << elem.back().lo << "," << elem.back().hi << "> ";
    }
  }
  if(!missing.empty()) {
    for(const auto &elem: missing) {
      for (size_t i = 0; i < rgx.variables.size() - 1; ++i) {
        ss2 << rgx.variables[i] << "|" << elem[i].lo << "," << elem[i].hi << ">, ";
      }
      ss2 << rgx.variables.back() << "|" << elem.back().lo << "," << elem.back().hi << "> ";
    }
  }
  INFO("Additional values yielded: " << ss1.str());
  INFO("Missing values not yielded: " << ss2.str());
  REQUIRE((additional.empty() && missing.empty()));

  return false;
}

TEST_CASE("All substrings", "[evaluation]") {

  const std::string pattern = "!x{.+}";
  std::vector<std::string> vars = {"x"};

  std::vector<RegexSubTest> cases = {
      {"abcde",{
                  {{0, 1}},{{0, 2}},{{0, 3}},
                  {{0, 4}},{{0, 5}},{{1, 2}},
                  {{1, 3}},{{1, 4}},{{1, 5}},
                  {{2, 3}},{{2, 4}},{{2, 5}},
                  {{3, 4}},{{3, 5}},{{4, 5}}
              }
      },
      {"!@#()",{
                    {{0, 1}},{{0, 2}},{{0, 3}},
                    {{0, 4}},{{0, 5}},{{1, 2}},
                    {{1, 3}},{{1, 4}},{{1, 5}},
                    {{2, 3}},{{2, 4}},{{2, 5}},
                    {{3, 4}},{{3, 5}},{{4, 5}}
                }
      },
      {"A3,.>",{
                    {{0, 1}},{{0, 2}},{{0, 3}},
                    {{0, 4}},{{0, 5}},{{1, 2}},
                    {{1, 3}},{{1, 4}},{{1, 5}},
                    {{2, 3}},{{2, 4}},{{2, 5}},
                    {{3, 4}},{{3, 5}},{{4, 5}}
                }
      },
      {"?/';]",{
                    {{0, 1}},{{0, 2}},{{0, 3}},
                    {{0, 4}},{{0, 5}},{{1, 2}},
                    {{1, 3}},{{1, 4}},{{1, 5}},
                    {{2, 3}},{{2, 4}},{{2, 5}},
                    {{3, 4}},{{3, 5}},{{4, 5}}
                }
      }
  };

  RegExOptions opts;


  SECTION("Normal mode", "[normal]") {
    opts.set_searching(false);
    for (auto const& subtest: cases) {
      test_searching({subtest.doc, pattern, vars, subtest.maps}, opts);
    }
  }

  SECTION("Searching mode", "[searching]") {
    opts.set_searching(true);
    for (auto const& subtest: cases) {
      test_searching({subtest.doc, pattern, vars, subtest.maps}, opts);
    }
  }
}

TEST_CASE("Character sets", "[searching]") {

  const std::string pattern = R"(!x{\D\d\w\W})";
  std::vector<std::string> vars = {"x"};

  std::vector<RegexSubTest> cases = {
      {"A1d_",{{{0,4}}}},
      {"A1dd", {}},
      {"A1__", {}},
      {"AAd_", {}},
      {"11d_", {}},
  };

  RegExOptions opts;

  SECTION("Normal mode", "[normal]") {
    opts.set_searching(false);
    for (auto const& subtest: cases) {
      test_searching({subtest.doc, pattern, vars, subtest.maps}, opts);
    }
  }

  SECTION("Searching mode", "[searching]") {
    opts.set_searching(true);
    for (auto const& subtest: cases) {
      test_searching({subtest.doc, pattern, vars, subtest.maps}, opts);
    }
  }
}

}  // namespace rematch::testing