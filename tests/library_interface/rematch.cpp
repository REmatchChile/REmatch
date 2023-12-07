#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/rematch.hpp"
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "evaluation/start_end_chars.hpp"

namespace rematch::testing {
using namespace REMatch;

void run_client_test(MatchIterator& match_iterator, std::vector<DummyMapping> expected_matches);
std::string get_mapping_info(DummyMapping mapping);

TEST_CASE("find method simple test") {
  std::string document = "This is a document";
  Regex regex = REMatch::compile("!x{doc|document}");
  std::unique_ptr<Match> match = regex.find(document);

  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == (Span){10, 13});
}

TEST_CASE("finditer method simple test") {
  std::string document = "This is a document";
  REMatch::Regex regex = REMatch::compile("!x{doc|document}");
  MatchIterator match_iterator = regex.finditer(document);

  std::unique_ptr<Match> match = match_iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == (Span){10, 13});
  
  match = match_iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == (Span){10, 18});

  match = match_iterator.next();
  REQUIRE(match == nullptr);
}

TEST_CASE("client interface with no matches") {
  std::string document = "This is a document";
  REMatch::Regex regex = REMatch::compile("!y{docx}");
  MatchIterator match_iterator = regex.finditer(document);

  std::unique_ptr<Match> match = match_iterator.next();
  REQUIRE(match == nullptr);
}

TEST_CASE("client interface returns empty match if there are no variables in regex") {
  std::string document = "This is a document";
  REMatch::Regex regex = REMatch::compile("a document");
  MatchIterator match_iterator = regex.finditer(document);

  std::unique_ptr<Match> match = match_iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->empty());
}

TEST_CASE("client interface with alternation") {
  std::string document = "This is a document";
  REMatch::Regex regex = REMatch::compile("!x{doc|document}");
  MatchIterator match_iterator = regex.finditer(document);

  std::unique_ptr<Match> match = match_iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == (Span){10, 13});
  
  match = match_iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->span("x") == (Span){10, 18});

  match = match_iterator.next();
  REQUIRE(match == nullptr);
}

TEST_CASE("client interface with + quantifier") {
  std::string document = "1100101100011101;";
  REMatch::Regex regex = REMatch::compile("!x{110(0|1)+};");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {0, 16}}}),
      DummyMapping({{"x", {6, 16}}}),
      DummyMapping({{"x", {12, 16}}})
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with * quantifier") {
  std::string document = "abab";
  REMatch::Regex regex = REMatch::compile("!x{a*!y{b+}a*}");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {1, 2}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {0, 2}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {1, 3}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {0, 3}}, {"y", {1, 2}}}),
      DummyMapping({{"x", {3, 4}}, {"y", {3, 4}}}),
      DummyMapping({{"x", {2, 4}}, {"y", {3, 4}}})
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with ? quantifier") {
  std::string document = "aabaaabba";
  REMatch::Regex regex = REMatch::compile("!x{a[ab]?b}");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
      DummyMapping({{"x", {1, 3}}}), DummyMapping({{"x", {0, 3}}}),
      DummyMapping({{"x", {5, 7}}}), DummyMapping({{"x", {4, 7}}}),
      DummyMapping({{"x", {5, 8}}})};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with specified range of repetitions") {
  std::string document = " google.org or google.fr ";
  REMatch::Regex regex = REMatch::compile(" !site{!name{\\w+}\\.\\w{2,3}} ");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"site", {1, 11}}, {"name", {1, 7}}}),
    DummyMapping({{"site", {15, 24}}, {"name", {15, 21}}})
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with char classes") {
  std::string document = "Regular Expression.";
  REMatch::Regex regex = REMatch::compile("!x{[A-Z][a-z]+}[ .]");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {8, 18}}}),
    DummyMapping({{"x", {0, 7}}})
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with short hand character classes (\\w)") {
  std::string document = "!variable1{a+}!variable2{b+}";
  REMatch::Regex regex = REMatch::compile("!!var{\\w+}\\{");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"var", {1, 10}}}),
    DummyMapping({{"var", {15, 24}}})
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with short hand character classes (\\s)") {
  std::string document = "aa  bb  cc";
  REMatch::Regex regex = REMatch::compile("!whitespace{\\S+\\s}");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"whitespace", {1, 3}}}),
    DummyMapping({{"whitespace", {0, 3}}}),
    DummyMapping({{"whitespace", {5, 7}}}),
    DummyMapping({{"whitespace", {4, 7}}})
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with negation") {
  std::string document = "a <span>certain</span> html <b>tag</b>";
  REMatch::Regex regex = REMatch::compile("!open{<[^/<>]*>}[^<]*!close{</[^<>]*>}");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"open", {2, 8}}, {"close", {15, 22}}}),
    DummyMapping({{"open", {28, 31}}, {"close", {34, 38}}})
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with start anchor") {
  std::string document = "aa";

  REMatch::Regex regex = REMatch::compile("^!x{a}");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {0, 1}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with end anchor") {
  std::string document = "document sent";

  REMatch::Regex regex = REMatch::compile("!x{ent}$");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {10, 13}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with anchors and alternation") {
  std::string document = "document sent";

  REMatch::Regex regex = REMatch::compile("!x{\\w}($| )");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {7, 8}}}),
    DummyMapping({{"x", {12, 13}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with anchors that result in an empty automata") {
  std::string document = "a";

  REMatch::Regex regex = REMatch::compile("$a");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {};

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with start and end anchors") {
  std::string document = "welcome";

  REMatch::Regex regex = REMatch::compile("^!x{welcome}$");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {0, 7}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with escape characters") {
  std::string document = "^!?\\a+*";
  REMatch::Regex regex = REMatch::compile("^\\^!\\?\\\\!x{a\\+}\\*$");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {4, 6}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with \\n in character set") {
  std::string document = "a\nb\nc";
  REMatch::Regex regex = REMatch::compile("!x{[^\\n]}.*!y{[\\n]}");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {0, 1}}, {"y", {1, 2}}}),
    DummyMapping({{"x", {0, 1}}, {"y", {3, 4}}}),
    DummyMapping({{"x", {2, 3}}, {"y", {3, 4}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

TEST_CASE("client interface with special characters inside a negated set") {
  std::string document = "a\fb\vc";
  REMatch::Regex regex = REMatch::compile("!x{[^\\f]}.*!y{[^\\v]}");
  MatchIterator match_iterator = regex.finditer(document);

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
  REMatch::Regex regex = REMatch::compile("!x{[\\t].*\\v.*[\\r\\n]}");
  MatchIterator match_iterator = regex.finditer(document);

  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {0, 5}}}),
    DummyMapping({{"x", {0, 7}}}),
  };

  run_client_test(match_iterator, expected_matches);
}

void run_client_test(MatchIterator& match_iterator, std::vector<DummyMapping> expected_matches) {
  std::unique_ptr<Match> match = match_iterator.next();

  std::ostringstream info_os;
  info_os << "Actual mappings:\n";

  while (match != nullptr) {
    DummyMapping mapping({});

    for (auto variable : match->variables()) {
      mapping.add_span(variable, match->span(variable));
    }
    info_os << get_mapping_info(mapping);
    INFO(info_os.str());
    REQUIRE(contains_mapping(expected_matches, mapping));
    remove_mapping_from_expected(expected_matches, mapping);

    match = match_iterator.next();
  }

  INFO(info_os.str());
  REQUIRE(expected_matches.size() == 0);
}

}  // namespace rematch::testing
