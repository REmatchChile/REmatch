#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>

namespace REmatch::testing {

TEST_CASE("multi match iterator returns the correct matches") {
  std::string pattern = "(^|[^\\w])!x{\\w+} !x{\\w+}($|[^\\w])";
  std::string document = "one two three";

  auto query = multi_reql(pattern);
  auto match_generator = query.finditer(document);

  auto it = match_generator.begin();
  auto end = match_generator.end();

  REQUIRE(it != end);
  auto match = *it;
  REQUIRE(match.spans("x") == std::vector<Span>{{0, 3}, {4, 7}});

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.spans("x") == std::vector<Span>{{4, 7}, {8, 13}});

  REQUIRE(++it == end);
}

}  // namespace REmatch::testing
