#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <set>

#include "REmatch/REmatch.hpp"

namespace REmatch::testing {

TEST_CASE("finditer w line by line wo anchors") {
  std::string document("0\n2\n4\n");
  auto query = reql("!x{.+}", Flags::LINE_BY_LINE);

  auto it = query.finditer(document);

  std::set<Span> actual_spans;

  for (auto match : it) {
    actual_spans.insert(match.span("x"));
  }

  std::set<Span> expected_spans = {{0, 1}, {2, 3}, {4, 5}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("finditer w line by line w start anchor") {
  std::string document("01\n34\n67\n");
  auto query = reql("^!x{.+}", Flags::LINE_BY_LINE);

  auto it = query.finditer(document);

  std::set<Span> actual_spans;

  for (const auto& match : it) {
    actual_spans.insert(match.span("x"));
  }

  std::set<Span> expected_spans = {{0, 1}, {0, 2}, {3, 4}, {3, 5}, {6, 7}, {6, 8}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("finditer w line by line w end anchor") {
  std::string document("01\n34\n67\n");
  auto query = reql("!x{.+}$", Flags::LINE_BY_LINE);

  auto it = query.finditer(document);

  std::set<Span> actual_spans;

  for (const auto& match : it) {
    actual_spans.insert(match.span("x"));
  }

  std::set<Span> expected_spans = {{1, 2}, {0, 2}, {4, 5}, {3, 5}, {7, 8}, {6, 8}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("findone w line by line wo anchors") {
  std::string document("\n12\n");
  auto query = reql("!x{.}", Flags::LINE_BY_LINE);

  auto match = query.findone(document);

  REQUIRE(match->span(0) == Span{1, 2});
}

TEST_CASE("findone w line by line w end anchor") {
  std::string document("\n12\n");
  auto query = reql("!x{.}$", Flags::LINE_BY_LINE);

  auto match = query.findone(document);

  REQUIRE(match->span(0) == Span{2, 3});
}

TEST_CASE("findone w line by line w start anchor") {
  std::string document("\na2\n12");
  auto query = reql("^!x{\\d}", Flags::LINE_BY_LINE);

  auto match = query.findone(document);

  REQUIRE(match->span(0) == Span{4, 5});
}

TEST_CASE("stream finditer w line by line wo anchors") {
  std::stringstream document("0\n2\n4\n");
  auto query = reql("!x{.+}", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto it = query.finditer(reader.get());

  std::set<Span> actual_spans;

  for (const auto& match : it) {
    actual_spans.insert(match.span("x"));
  }

  std::set<Span> expected_spans = {{0, 1}, {2, 3}, {4, 5}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("stream finditer w line by line w start anchor") {
  std::stringstream document("01\n34\n67\n");
  auto query = reql("^!x{.+}", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto it = query.finditer(reader.get());

  std::set<Span> actual_spans;

  for (const auto& match : it) {
    actual_spans.insert(match.span("x"));
  }

  std::set<Span> expected_spans = {{0, 1}, {0, 2}, {3, 4}, {3, 5}, {6, 7}, {6, 8}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("stream finditer w line by line w end anchor") {
  std::stringstream document("01\n34\n67\n");
  auto query = reql("!x{.+}$", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto it = query.finditer(reader.get());

  std::set<Span> actual_spans;

  for (const auto& match : it) {
    actual_spans.insert(match.span("x"));
  }

  std::set<Span> expected_spans = {{1, 2}, {0, 2}, {4, 5}, {3, 5}, {7, 8}, {6, 8}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("stream findone w line by line wo anchors") {
  std::stringstream document("\n12\n");
  auto query = reql("!x{.}", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto match = query.findone(reader.get());

  REQUIRE(match->span(0) == Span{1, 2});
}

TEST_CASE("stream findone w line by line w end anchor") {
  std::stringstream document("\n12\n");
  auto query = reql("!x{.}$", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto match = query.findone(reader.get());

  REQUIRE(match->span(0) == Span{2, 3});
}

TEST_CASE("stream findone w line by line w start anchor") {
  std::string document("\na2\n12");
  auto query = reql("^!x{\\d}", Flags::LINE_BY_LINE);

  auto match = query.findone(document);

  REQUIRE(match->span(0) == Span{4, 5});
}

TEST_CASE("findone empty lines") {
  std::string document("\n\n\n");
  auto query = reql("!x{.}", Flags::LINE_BY_LINE);

  REQUIRE_FALSE(query.findone(document).has_value());
}

TEST_CASE("multi finditer w line by line wo anchors") {
  std::string document("01\n34\n67\n");
  auto query = multi_reql("!x{.}!x{.}", Flags::LINE_BY_LINE);

  auto it = query.finditer(document);

  std::set<std::vector<Span>> actual_spans;

  for (const auto& match : it) {
    actual_spans.insert(match.spans("x"));
  }

  std::set<std::vector<Span>> expected_spans = {
      {{0, 1}, {1, 2}}, {{3, 4}, {4, 5}}, {{6, 7}, {7, 8}}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("multi finditer w line by line w start anchor") {
  std::string document("01\n34\n67\n");
  auto query = multi_reql("^!x{.}+", Flags::LINE_BY_LINE);

  auto it = query.finditer(document);

  std::set<std::vector<Span>> actual_spans;

  for (const auto& match : it) {
    actual_spans.insert(match.spans("x"));
  }

  std::set<std::vector<Span>> expected_spans = {
      {{0, 1}}, {{0, 1}, {1, 2}}, {{3, 4}}, {{3, 4}, {4, 5}}, {{6, 7}}, {{6, 7}, {7, 8}}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("multi finditer w line by line w end anchor") {
  std::string document("01\n34\n67\n");
  auto query = multi_reql("!x{.}+$", Flags::LINE_BY_LINE);

  auto it = query.finditer(document);

  std::set<std::vector<Span>> actual_spans;

  for (const auto& match : it) {
    actual_spans.insert(match.spans("x"));
  }

  std::set<std::vector<Span>> expected_spans = {
      {{0, 1}, {1, 2}}, {{1, 2}}, {{3, 4}, {4, 5}}, {{4, 5}}, {{6, 7}, {7, 8}}, {{7, 8}}};
  REQUIRE(actual_spans == expected_spans);
}

TEST_CASE("multi findone w line by line wo anchors") {
  std::string document("\n12\n");
  auto query = multi_reql("!x{.}+", Flags::LINE_BY_LINE);

  auto match = query.findone(document);

  std::vector<Span> expected_match = {{1, 2}};
  REQUIRE(match->spans(0) == expected_match);
}

TEST_CASE("multi findone w line by line w end anchor") {
  std::string document("\n12\n");
  auto query = multi_reql("!x{.}$", Flags::LINE_BY_LINE);

  auto match = query.findone(document);

  std::vector<Span> expected_match = {{2, 3}};
  REQUIRE(match->spans(0) == expected_match);
}

TEST_CASE("multi findone w line by line w start anchor") {
  std::string document("\na23\n56");
  auto query = multi_reql("^!x{\\d}!x{.}+", Flags::LINE_BY_LINE);

  auto match = query.findone(document);

  std::vector<Span> expected_match = {{5, 6}, {6, 7}};
  REQUIRE(match->spans(0) == expected_match);
}

TEST_CASE("an exception is thrown when a line does not fit in the buffer") {
  std::stringstream document("when you try your best");
  auto query = reql("!x{\\s}", Flags::LINE_BY_LINE, DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                    DEFAULT_MAX_DETERMINISTIC_STATES, 10);

  auto reader = std::make_unique<FStreamReader>(document);
  auto iter = query.finditer(reader.get());

  REQUIRE_THROWS(iter.begin());
}

TEST_CASE("stream single line") {
  std::stringstream document("when you try your best");
  auto query = reql("!x{.}$", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto iter = query.finditer(reader.get());

  auto m = iter.begin();

  REQUIRE((*m).group("x") == "t");
}

TEST_CASE("stream multi finditer w lbl wo anchors") {
  std::stringstream document("012\n4\n6");
  auto query = multi_reql("!x{.}+", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto iter = query.finditer(reader.get());

  std::set<std::vector<Span>> actual;
  for (auto m : iter) {
    actual.insert(m.spans("x"));
  }

  std::set<std::vector<Span>> expected = {{{0, 1}}, {{0, 1}, {1, 2}}, {{0, 1}, {1, 2}, {2, 3}},
                                          {{1, 2}}, {{1, 2}, {2, 3}}, {{2, 3}},
                                          {{4, 5}}, {{6, 7}}};
  REQUIRE(actual == expected);
}

TEST_CASE("stream multi finditer w lbl w start anchor") {
  std::stringstream document("012\n45\n78\n");
  auto query = multi_reql("^!x{.}+", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto iter = query.finditer(reader.get());

  std::stringstream info;
  std::set<std::vector<Span>> actual;
  for (const auto& m : iter) {
    actual.insert(m.spans("x"));
    for (auto s : m.spans("x")) {
      info << "{" << s.first << ", " << s.second << "} ";
    }
    info << "\n";
  }

  std::set<std::vector<Span>> expected = {
      {{0, 1}},         {{0, 1}, {1, 2}}, {{0, 1}, {1, 2}, {2, 3}}, {{4, 5}},
      {{4, 5}, {5, 6}}, {{7, 8}},         {{7, 8}, {8, 9}}};
  INFO(info.str());
  REQUIRE(actual == expected);
}

TEST_CASE("stream multi finditer w lbl w end anchor") {
  std::stringstream document("012\n45\n78\n");
  auto query = multi_reql("!x{.}+$", Flags::LINE_BY_LINE);

  auto reader = std::make_unique<FStreamReader>(document);
  auto iter = query.finditer(reader.get());

  std::stringstream info;
  std::set<std::vector<Span>> actual;
  for (auto m : iter) {
    actual.insert(m.spans("x"));
    for (auto s : m.spans("x")) {
      info << "{" << s.first << ", " << s.second << "} ";
    }
    info << "\n";
  }

  std::set<std::vector<Span>> expected = {
      {{2, 3}},         {{1, 2}, {2, 3}}, {{0, 1}, {1, 2}, {2, 3}}, {{5, 6}},
      {{4, 5}, {5, 6}}, {{8, 9}},         {{7, 8}, {8, 9}}};
  INFO(info.str());
  REQUIRE(actual == expected);
}

}  // namespace REmatch::testing
