#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>

namespace REmatch::testing {

TEST_CASE("multi regex findone method returns the first match correctly") {
  std::string pattern = "^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$";
  std::string document = "aaa aa";
  auto query = multi_reql(pattern);
  std::unique_ptr<MultiMatch> match = query.findone(document);

  std::vector<Span> expected_spans = {{0, 3}, {4, 6}};
  REQUIRE(match->spans(0) == expected_spans);
  REQUIRE(match->spans("x") == expected_spans);
}

TEST_CASE("multi regex returns nullptr when there are no matches") {
  std::string pattern = "!x{b}!x{b}";
  std::string document = "aa";
  auto query = multi_reql(pattern);
  auto match_iterator = query.finditer(document);

  REQUIRE(match_iterator.next() == nullptr);
}

TEST_CASE("multi regex returns an empty match when nothing is captured") {
  std::string pattern = "!x{b}|a";
  std::string document = "a";
  auto query = multi_reql(pattern);
  auto match_iterator = query.finditer(document);

  std::unique_ptr<MultiMatch> match = match_iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->empty());
}

TEST_CASE("multi regex returns the expected result when using alternation") {
  std::string pattern = "!x{b}|!y{b}";
  std::string document = "b";
  auto query = multi_reql(pattern);
  auto actual_matches = query.findall(document);

  for (auto& match : actual_matches) {
    const auto x_spans = match->spans("x");
    const auto y_spans = match->spans("y");

    if (x_spans.empty()) {
      REQUIRE(y_spans == std::vector<Span>{{0, 1}});
    } else {
      REQUIRE(x_spans == std::vector<Span>{{0, 1}});
      REQUIRE(y_spans.empty());
    }
  }
}

TEST_CASE("multi regex returns the correct result when using quantifiers") {
  std::string pattern = "!x{b}+";
  std::string document = "bb";
  auto query = multi_reql(pattern);
  auto iterator = query.finditer(document);

  auto match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{0, 1}});

  match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{1, 2}});

  match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->spans("x") == std::vector<Span>{{0, 1}, {1, 2}});

  match = iterator.next();
  REQUIRE(match == nullptr);
}

TEST_CASE("multi regex returns the correct result when using char classes") {
  std::string pattern =
      "(\\n|^) !line{[0-9]+(\\* |  )!com{\\w+}( !args{[^ \\n]+})*}(\\n|$)";
  std::string document =
      " 1  git pull\n 2  python3 --version\n 3* apt install texlive";

  auto query = multi_reql(pattern);
  auto iterator = query.finditer(document);

  auto match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("com") == std::vector<std::string>{"git"});
  REQUIRE(match->groups("args") == std::vector<std::string>{"pull"});

  match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("com") == std::vector<std::string>{"python3"});
  REQUIRE(match->groups("args") == std::vector<std::string>{"--version"});

  match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("com") == std::vector<std::string>{"apt"});
  REQUIRE(match->groups("args") ==
          std::vector<std::string>{"install", "texlive"});

  match = iterator.next();
  REQUIRE(match == nullptr);
}

TEST_CASE("multi regex returns the correct result when using anchors") {
  std::string pattern = "(^|;)!row{(-|!mark{O|X})(,(!mark{O|X}|-))+}($|;)";
  std::string document = "O,-,X;-,X,O;O,-,-";
  auto query = multi_reql(pattern);
  auto iterator = query.finditer(document);

  auto match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("row") == std::vector<std::string>{"O,-,X"});
  REQUIRE(match->spans("mark") == std::vector<Span>{{0, 1}, {4, 5}});

  match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("row") == std::vector<std::string>{"-,X,O"});
  REQUIRE(match->spans("mark") == std::vector<Span>{{8, 9}, {10, 11}});

  match = iterator.next();
  REQUIRE(match != nullptr);
  REQUIRE(match->groups("row") == std::vector<std::string>{"O,-,-"});
  REQUIRE(match->spans("mark") == std::vector<Span>{{12, 13}});

  match = iterator.next();
  REQUIRE(match == nullptr);
}

TEST_CASE("multi regex finditer method returns iterator correctly") {
  std::string pattern = "(^|(\\. ))!x{!y{\\w+}([^\\w.]+!y{\\w+})*}\\.";
  std::string document = "Hi friend. Take care.";
  auto query = multi_reql(pattern);
  auto match_iterator = query.finditer(document);

  std::vector<std::map<std::string, std::vector<std::string>>>
      expected_multispans = {{
                                 {"x", {"Hi friend"}},
                                 {"y", {"Hi", "friend"}},
                             },
                             {
                                 {"x", {"Take care"}},
                                 {"y", {"Take", "care"}},
                             }};

  auto match = match_iterator.next();
  REQUIRE(match->groups("x") == expected_multispans[0]["x"]);
  REQUIRE(match->groups("y") == expected_multispans[0]["y"]);

  match = match_iterator.next();
  REQUIRE(match->groups("x") == expected_multispans[1]["x"]);
  REQUIRE(match->groups("y") == expected_multispans[1]["y"]);

  REQUIRE(match_iterator.next() == nullptr);
}

TEST_CASE("multi regex check method returns true when there is an output") {
  std::string pattern = "^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$";
  std::string document = "aaa aa";
  auto query = multi_reql(pattern);

  REQUIRE(query.check(document));
}

TEST_CASE("multi regex check method returns true when there is no output") {
  std::string pattern = "^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$";
  std::string document = "bbb bb";
  auto query = multi_reql(pattern);

  REQUIRE_FALSE(query.check(document));
}

}  // namespace REmatch::testing