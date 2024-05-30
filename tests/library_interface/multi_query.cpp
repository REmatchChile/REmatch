#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/multi_query.hpp"
#include "library_interface/query.hpp"

namespace rematch::testing {

TEST_CASE("multi regex findone method returns the first match correctly") {
  std::string pattern = "^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$";
  std::string document = "aaa aa";
  auto regex = MultiQuery(pattern);
  std::unique_ptr<MultiMatch> match = regex.findone(document);

  std::vector<Span> expected_spans = {{0, 3}, {4, 6}};
  REQUIRE(match->spans(0) == expected_spans);
  REQUIRE(match->spans("x") == expected_spans);
}

TEST_CASE("multi regex returns nullptr when there are no matches") {
  std::string pattern = "!x{b}!x{b}";
  std::string document = "aa";
  auto regex = MultiQuery(pattern);
  std::unique_ptr<MultiMatchIterator> match_iterator = regex.finditer(document);

  REQUIRE(match_iterator->next() == nullptr);
}

TEST_CASE("multi regex returns an empty match when nothing is captured") {
  std::string pattern = "!x{b}|a";
  std::string document = "a";
  auto regex = MultiQuery(pattern);
  std::unique_ptr<MultiMatchIterator> match_iterator = regex.finditer(document);

  std::unique_ptr<MultiMatch> match = match_iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->empty());
}

TEST_CASE("multi regex returns the expected result when using alternation") {
  std::string pattern = "!x{b}|!y{b}";
  std::string document = "b";
  auto regex = MultiQuery(pattern);
  auto iterator = regex.finditer(document);

  auto match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{0, 1}});
  REQUIRE(match->spans("y").empty());

  match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x").empty());
  REQUIRE(match->spans("y") == std::vector<Span>{{0, 1}});

  match = iterator->next();
  REQUIRE(match == nullptr);
}

TEST_CASE("multi regex returns the correct result when using quantifiers") {
  std::string pattern = "!x{b}+";
  std::string document = "bb";
  auto regex = MultiQuery(pattern);
  auto iterator = regex.finditer(document);

  auto match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{0, 1}});

  match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{1, 2}});

  match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{0, 1}, {1, 2}});

  match = iterator->next();
  REQUIRE(match == nullptr);
}

TEST_CASE("multi regex returns the correct result when using char classes") {
  std::string pattern =
      "(\\n|^) !line{[0-9]+(\\* |  )!com{\\w+}( !args{[^ \\n]+})*}(\\n|$)";
  std::string document =
      " 1  git pull\n 2  python3 --version\n 3* apt install texlive";

  auto regex = MultiQuery(pattern);
  auto iterator = regex.finditer(document);

  auto match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("com") == std::vector<std::string>{"git"});
  REQUIRE(match->groups("args") == std::vector<std::string>{"pull"});

  match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("com") == std::vector<std::string>{"python3"});
  REQUIRE(match->groups("args") == std::vector<std::string>{"--version"});

  match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("com") == std::vector<std::string>{"apt"});
  REQUIRE(match->groups("args") ==
          std::vector<std::string>{"install", "texlive"});

  match = iterator->next();
  REQUIRE(match == nullptr);
}

TEST_CASE("multi regex returns the correct result when using anchors") {
  std::string pattern = "(^|;)!row{(-|!mark{O|X})(,(!mark{O|X}|-))+}($|;)";
  std::string document = "O,-,X;-,X,O;O,-,-";
  auto regex = MultiQuery(pattern);
  auto iterator = regex.finditer(document);

  auto match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("row") == std::vector<std::string>{"O,-,X"});
  REQUIRE(match->spans("mark") == std::vector<Span>{{0, 1}, {4, 5}});

  match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("row") == std::vector<std::string>{"-,X,O"});
  REQUIRE(match->spans("mark") == std::vector<Span>{{8, 9}, {10, 11}});

  match = iterator->next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("row") == std::vector<std::string>{"O,-,-"});
  REQUIRE(match->spans("mark") == std::vector<Span>{{12, 13}});

  match = iterator->next();
  REQUIRE(match == nullptr);
}

TEST_CASE("multi regex finditer method returns iterator correctly") {
  std::string pattern = "(^|(\\. ))!x{!y{\\w+}([^\\w.]+!y{\\w+})*}\\.";
  std::string document = "Hi friend. Take care.";
  auto regex = MultiQuery(pattern);
  std::unique_ptr<MultiMatchIterator> match_iterator = regex.finditer(document);

  std::vector<std::map<std::string, std::vector<std::string>>>
      expected_multispans = {{
                                 {"x", {"Hi friend"}},
                                 {"y", {"Hi", "friend"}},
                             },
                             {
                                 {"x", {"Take care"}},
                                 {"y", {"Take", "care"}},
                             }};

  std::unique_ptr<MultiMatch> match = match_iterator->next();
  REQUIRE(match->groups("x") == expected_multispans[0]["x"]);
  REQUIRE(match->groups("y") == expected_multispans[0]["y"]);

  match = match_iterator->next();
  REQUIRE(match->groups("x") == expected_multispans[1]["x"]);
  REQUIRE(match->groups("y") == expected_multispans[1]["y"]);

  REQUIRE(match_iterator->next() == nullptr);
}

TEST_CASE("multi regex check method returns true when there is an output") {
  std::string pattern = "^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$";
  std::string document = "aaa aa";
  auto regex = MultiQuery(pattern);

  REQUIRE(regex.check(document));
}

TEST_CASE("multi regex check method returns true when there is no output") {
  std::string pattern = "^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$";
  std::string document = "bbb bb";
  auto regex = MultiQuery(pattern);

  REQUIRE_FALSE(regex.check(document));
}

}  // namespace rematch::testing
