#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {

TEST_CASE("find method simple test") {
  std::string document = "This is a document";
  std::string pattern = "!x{doc|document}";
  Query regex = reql(pattern);
  auto match = regex.findone(document);

  REQUIRE(match.span("x") == Span(10, 13));
}

TEST_CASE("finditer method simple test") {
  std::string document = "This is a document";
  std::string pattern = "!x{doc|document}";
  Query regex = reql(pattern);
  auto match_generator = regex.finditer(document);
  auto it = match_generator.begin();
  auto end = match_generator.end();

  REQUIRE(it != end);
  Match match = *it;
  REQUIRE(match.span("x") == Span(10, 13));

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.span("x") == Span(10, 18));

  REQUIRE(++it == end);
}

TEST_CASE("client interface with no matches") {
  std::string document = "This is a document";
  std::string pattern = "!y{docx}";
  Query regex = reql(pattern);
  auto match_generator = regex.finditer(document);

  REQUIRE(match_generator.begin() == match_generator.end());
}

TEST_CASE(
    "client interface returns empty match if there are no variables in regex") {
  std::string document = "This is a document";
  std::string pattern = "a document";
  Query regex = reql(pattern);
  auto match_generator = regex.finditer(document);
  auto it = match_generator.begin();

  Match match = *it;
  REQUIRE(it != match_generator.end());
  REQUIRE(match.empty());
}

TEST_CASE("client interface with alternation") {
  std::string document = "This is a document";
  std::string pattern = "!x{doc|document}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);
  auto it = match_iterator.begin();
  auto end = match_iterator.end();

  REQUIRE(it != end);
  Match match = *it;
  REQUIRE(match.span("x") == Span(10, 13));

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.span("x") == Span(10, 18));

  REQUIRE(++it == end);
}

TEST_CASE("client interface with + quantifier") {
  std::string document = "1100101100011101;";
  std::string pattern = "!x{110(0|1)+};";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 16}}}), DummyMapping({{"x", {6, 16}}}),
      DummyMapping({{"x", {12, 16}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with * quantifier") {
  std::string document = "abab";
  std::string pattern = "!x{a*!y{b+}a*}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {1, 2}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {0, 2}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {1, 3}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {0, 3}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {3, 4}}, {"y", {3, 4}}}),
      DummyMapping({{"x", {2, 4}}, {"y", {3, 4}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with ? quantifier") {
  std::string document = "aabaaabba";
  std::string pattern = "!x{a[ab]?b}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {1, 3}}}), DummyMapping({{"x", {0, 3}}}),
      DummyMapping({{"x", {5, 7}}}), DummyMapping({{"x", {4, 7}}}),
      DummyMapping({{"x", {5, 8}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with specified range of repetitions") {
  std::string document = " google.org or google.fr ";
  std::string pattern = " !site{!name{\\w+}\\.\\w{2,3}} ";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"site", {1, 11}}, {"name", {1, 7}}}),
      DummyMapping({{"site", {15, 24}}, {"name", {15, 21}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with char classes") {
  std::string document = "Regular Expression.";
  std::string pattern = "!x{[A-Z][a-z]+}[ .]";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {DummyMapping({{"x", {8, 18}}}),
                                                DummyMapping({{"x", {0, 7}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with short hand character classes (\\w)") {
  std::string document = "!variable1{a+}!variable2{b+}";
  std::string pattern = "!!var{\\w+}\\{";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"var", {1, 10}}}), DummyMapping({{"var", {15, 24}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with short hand character classes (\\s)") {
  std::string document = "aa  bb  cc";
  std::string pattern = "!whitespace{\\S+\\s}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"whitespace", {1, 3}}}),
      DummyMapping({{"whitespace", {0, 3}}}),
      DummyMapping({{"whitespace", {5, 7}}}),
      DummyMapping({{"whitespace", {4, 7}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with negation") {
  std::string document = "a <span>certain</span> html <b>tag</b>";
  std::string pattern = "!open{<[^/<>]*>}[^<]*!close{</[^<>]*>}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"open", {2, 8}}, {"close", {15, 22}}}),
      DummyMapping({{"open", {28, 31}}, {"close", {34, 38}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with start anchor") {
  std::string document = "aa";

  std::string pattern = "^!x{a}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 1}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with end anchor") {
  std::string document = "document sent";

  std::string pattern = "!x{ent}$";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {10, 13}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with anchors and alternation") {
  std::string document = "document sent";

  std::string pattern = "!x{\\w}($| )";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {7, 8}}}),
      DummyMapping({{"x", {12, 13}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with anchors that result in an empty automata") {
  std::string document = "a";

  std::string pattern = "$a";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with start and end anchors") {
  std::string document = "welcome";

  std::string pattern = "^!x{welcome}$";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 7}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with escape characters") {
  std::string document = "^!?\\a+*";
  std::string pattern = "^\\^!\\?\\\\!x{a\\+}\\*$";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {4, 6}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with \\n in character set") {
  std::string document = "a\nb\nc";
  std::string pattern = "!x{[^\\n]}.*!y{[\\n]}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 1}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {0, 1}}, {"y", {3, 4}}}),
      DummyMapping({{"x", {2, 3}}, {"y", {3, 4}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with special characters inside a negated set") {
  std::string document = "a\fb\vc";
  std::string pattern = "!x{[^\\f]}.*!y{[^\\v]}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 1}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {0, 1}}, {"y", {2, 3}}}),
      DummyMapping({{"x", {0, 1}}, {"y", {4, 5}}}),
      DummyMapping({{"x", {2, 3}}, {"y", {4, 5}}}),
      DummyMapping({{"x", {3, 4}}, {"y", {4, 5}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with special characters inside a character set") {
  std::string document = "\ta\va\ra\na";
  std::string pattern = "!x{[\\t].*\\v.*[\\r\\n]}";
  Query regex = reql(pattern);
  auto match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 5}}}),
      DummyMapping({{"x", {0, 7}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("the finditer function works correctly") {
  std::string document = "aaba";
  std::string pattern = "!x{a+}";

  auto query = reql(pattern);
  auto match_iterator = query.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 1}}}),
      DummyMapping({{"x", {0, 2}}}),
      DummyMapping({{"x", {1, 2}}}),
      DummyMapping({{"x", {3, 4}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("the regex can be evaluated more than once") {
  std::string document = "aba";
  std::string pattern = "!x{a+}";
  Query regex = reql(pattern);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 1}}}),
      DummyMapping({{"x", {2, 3}}}),
  };

  auto match_iterator = regex.finditer(document);
  run_client_test(match_iterator, expected_matches);

  auto match_iterator2 = regex.finditer(document);
  run_client_test(match_iterator2, expected_matches);
}

}  // namespace REmatch::testing
