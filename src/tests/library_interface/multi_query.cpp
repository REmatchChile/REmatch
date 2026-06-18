#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "REmatch/REmatch.hpp"

namespace REmatch::testing {

TEST_CASE("multi regex findone method returns the first match correctly") {
  std::string pattern = "^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$";
  std::string document = "aaa aa";
  auto query = multi_reql(pattern);
  auto match = query.findone(document);

  std::vector<Span> expected_spans = {{0, 3}, {4, 6}};
  REQUIRE(match->spans(0) == expected_spans);
  REQUIRE(match->spans("x") == expected_spans);
}

TEST_CASE("multi regex behaves correctly when there are no matches") {
  std::string pattern = "!x{b}!x{b}";
  std::string document = "aa";
  auto query = multi_reql(pattern);
  auto match_generator = query.finditer(document);
  auto it = match_generator.begin();

  REQUIRE(it == match_generator.end());
}

TEST_CASE("multi regex returns an empty match when nothing is captured") {
  std::string pattern = "!x{b}|a";
  std::string document = "a";
  auto query = multi_reql(pattern);
  auto match_generator = query.finditer(document);
  auto it = match_generator.begin();

  auto match = *it;
  REQUIRE(match.empty());
}

TEST_CASE("multi regex returns the expected result when using alternation") {
  std::string pattern = "!x{b}|!y{b}";
  std::string document = "b";
  auto query = multi_reql(pattern);
  auto actual_matches = query.findall(document);

  for (auto& match : actual_matches) {
    const auto x_spans = match.spans("x");
    const auto y_spans = match.spans("y");

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
  auto match_generator = query.finditer(document);
  auto it = match_generator.begin();
  auto end = match_generator.end();

  REQUIRE(it != end);
  auto match = *it;
  REQUIRE(match.spans("x") == std::vector<Span>{{0, 1}});

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.spans("x") == std::vector<Span>{{1, 2}});

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.spans("x") == std::vector<Span>{{0, 1}, {1, 2}});

  REQUIRE(++it == end);
}

TEST_CASE("multi regex returns the correct result when using char classes") {
  std::string pattern = R"((\n|^) !line{[0-9]+(\* |  )!com{\w+}( !args{[^ \n]+})*}(\n|$))";
  std::string document = " 1  git pull\n 2  python3 --version\n 3* apt install texlive";

  auto query = multi_reql(pattern);
  auto match_generator = query.finditer(document);
  auto it = match_generator.begin();
  auto end = match_generator.end();

  REQUIRE(it != end);
  auto match = *it;
  REQUIRE(match.groups("com") == std::vector<std::string>{"git"});
  REQUIRE(match.groups("args") == std::vector<std::string>{"pull"});

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.groups("com") == std::vector<std::string>{"python3"});
  REQUIRE(match.groups("args") == std::vector<std::string>{"--version"});

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.groups("com") == std::vector<std::string>{"apt"});
  REQUIRE(match.groups("args") == std::vector<std::string>{"install", "texlive"});

  REQUIRE(++it == end);
}

TEST_CASE("multi regex returns the correct result when using anchors") {
  std::string pattern = "(^|;)!row{(-|!mark{O|X})(,(!mark{O|X}|-))+}($|;)";
  std::string document = "O,-,X;-,X,O;O,-,-";
  auto query = multi_reql(pattern);
  auto match_generator = query.finditer(document);
  auto it = match_generator.begin();
  auto end = match_generator.end();

  REQUIRE(it != end);
  auto match = *it;
  REQUIRE(match.groups("row") == std::vector<std::string>{"O,-,X"});
  REQUIRE(match.spans("mark") == std::vector<Span>{{0, 1}, {4, 5}});

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.groups("row") == std::vector<std::string>{"-,X,O"});
  REQUIRE(match.spans("mark") == std::vector<Span>{{8, 9}, {10, 11}});

  REQUIRE(++it != end);
  match = *(it);
  REQUIRE(match.groups("row") == std::vector<std::string>{"O,-,-"});
  REQUIRE(match.spans("mark") == std::vector<Span>{{12, 13}});

  REQUIRE(++it == end);
}

TEST_CASE("multi regex finditer method returns iterator correctly") {
  std::string pattern = R"((^|(\. ))!x{!y{\w+}([^\w.]+!y{\w+})*}\.)";
  std::string document = "Hi friend. Take care.";
  auto query = multi_reql(pattern);
  auto match_generator = query.finditer(document);
  auto it = match_generator.begin();
  auto end = match_generator.end();

  std::vector<std::map<std::string, std::vector<std::string>>> expected_multispans = {
      {
          {"x", {"Hi friend"}},
          {"y", {"Hi", "friend"}},
      },
      {
          {"x", {"Take care"}},
          {"y", {"Take", "care"}},
      }};

  auto match = *it;
  REQUIRE(match.groups("x") == expected_multispans[0]["x"]);
  REQUIRE(match.groups("y") == expected_multispans[0]["y"]);

  match = *(++it);
  REQUIRE(match.groups("x") == expected_multispans[1]["x"]);
  REQUIRE(match.groups("y") == expected_multispans[1]["y"]);

  REQUIRE(++it == end);
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

TEST_CASE("multi regex submatch function") {
  std::string pattern = "(!x{a+}!x{b+}){2}";
  std::string document = "abbabba";
  auto query = multi_reql(pattern);

  auto match = query.findone(document);

  auto submatch = match->submatch(Span(0, 3));
  std::vector<Span> expected = {{0, 1}, {1, 3}};
  REQUIRE(submatch.spans("x") == expected);
}

TEST_CASE("multi regex check lbl") {
  std::string pattern = "!x{.}!x{.}";
  std::string document = "0\n2\n4\n7";
  auto query = multi_reql(pattern, Flags::LINE_BY_LINE);

  REQUIRE_FALSE(query.check(document));
}

TEST_CASE("multi stream check lbl") {
  std::string pattern = "!x{.}!x{.}";
  std::string document = "0\n2\n4\n7";
  std::stringstream document_stream(document);
  auto reader = std::make_unique<FStreamReader>(document_stream);
  auto query = multi_reql(pattern, Flags::LINE_BY_LINE);

  REQUIRE_FALSE(query.check(reader.get()));
}

TEST_CASE("multi no capture") {
  std::string pattern = "a";
  std::string document = "aabbaa";
  auto query = multi_reql(pattern);
  auto matches = query.findall(document);

  REQUIRE(matches.size() == 1);
  REQUIRE(matches[0].empty());
}

TEST_CASE("multi no capture lbl") {
  std::string pattern = "a";
  std::string document = "aabbaa";
  auto query = multi_reql(pattern, Flags::LINE_BY_LINE);
  auto matches = query.findall(document);

  REQUIRE(matches.size() == 1);
  REQUIRE(matches[0].empty());
}

TEST_CASE("stream multi no capture") {
  std::string pattern = "a";
  std::string document = "aabbaa";
  std::stringstream document_stream(document);
  auto reader = std::make_unique<FStreamReader>(document_stream);
  auto query = multi_reql(pattern);
  auto matches = query.findall(reader.get());

  REQUIRE(matches.size() == 1);
  REQUIRE(matches[0].empty());
}

TEST_CASE("stream multi no capture lbl") {
  std::string pattern = "a";
  std::string document = "aabbaa";
  std::stringstream document_stream(document);
  auto reader = std::make_unique<FStreamReader>(document_stream);
  auto query = multi_reql(pattern, Flags::LINE_BY_LINE);
  auto matches = query.findall(reader.get());

  REQUIRE(matches.size() == 1);
  REQUIRE(matches[0].empty());
}

}  // namespace REmatch::testing
