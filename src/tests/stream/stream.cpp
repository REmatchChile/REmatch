#include "evaluation/stream.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <set>
#undef private
#include <sstream>

#include "REmatch/REmatch.hpp"

namespace REmatch::testing {

TEST_CASE("The reader works correctly") {
  std::string document("You don't know about me.");
  std::istringstream string_stream(document);
  auto reader = std::make_unique<FStreamReader>(string_stream);

  char a_stream;
  for (char& a_doc : document) {
    REQUIRE(reader->read(a_stream));
    REQUIRE(a_stream == a_doc);
  }

  REQUIRE(reader->read(a_stream) == false);
}

TEST_CASE("The stream object works correctly") {
  std::string document("You don't know about me.");
  std::istringstream string_stream(document);
  FStreamReader reader(string_stream);

  std::ostringstream out_stream;

  Stream stream(&reader);

  char a;
  while (stream.read(a)) {
    out_stream << a;
  }

  std::string read_string = out_stream.str();

  REQUIRE(read_string.size() == document.size() + 1);
  REQUIRE(std::string(read_string, 0, document.size()) == document);
}

TEST_CASE("The circular buffer works correctly") {

  CircularBuffer cbuffer(5);
  std::string document("01234567");

  for (auto& a : document) {
    cbuffer.insert(a);
  }

  std::string buffer_str = cbuffer.substr({4, 7});
  std::string expected_str = "456";

  REQUIRE(buffer_str == expected_str);
}

TEST_CASE("The increase buffer function works correctly") {

  CircularBuffer cbuffer(5);
  cbuffer.increase_size();

  std::string document("012345678");

  for (auto& a : document) {
    cbuffer.insert(a);
  }

  // we should get the whole document
  std::string buffer_str = cbuffer.substr({0, 9});
  REQUIRE(buffer_str == document);
}

TEST_CASE("reql works with the regex \\d\\d") {
  std::string pattern = R"(!x{\d\d})";
  std::string document = "01234567";
  std::istringstream string_stream(document);

  Query query = reql(pattern);
  FStreamReader reader(string_stream);

  auto iter = query.finditer(&reader);

  std::vector<Span> actual_spans;
  std::vector<std::string> actual_strings;

  for (const auto& m : iter) {
    actual_spans.push_back(m.span("x"));
    actual_strings.push_back(m.group("x"));
  }

  std::vector<Span> expected_spans = {{0, 2}, {1, 3}, {2, 4}, {3, 5}, {4, 6}, {5, 7}, {6, 8}};
  std::vector<std::string> expected_groups = {"01", "12", "23", "34", "45", "56", "67"};
  REQUIRE(actual_spans == expected_spans);
  REQUIRE(actual_strings == expected_groups);
}

TEST_CASE("reql works when evaluating a file (without filtering)") {
  std::string pattern(R"((^|\W)!w{\w+}($|\W))");

  std::string document("You know about me.");
  std::istringstream string_stream(document);

  Query query = reql(pattern);
  FStreamReader reader(string_stream);
  auto iter = query.finditer(&reader);

  std::vector<Span> actual_spans;
  std::vector<std::string> actual_strings;

  for (const auto& m : iter) {
    actual_spans.push_back(m.span("w"));
    actual_strings.push_back(m.group("w"));
  }

  std::vector<Span> expected_spans = {{0, 3}, {4, 8}, {9, 14}, {15, 17}};
  std::vector<std::string> expected_groups = {"You", "know", "about", "me"};

  REQUIRE(actual_spans == expected_spans);
  REQUIRE(actual_strings == expected_groups);
}

TEST_CASE("reql works correctly when evaluating a file (with filtering)") {
  std::string pattern(R"((=|\W)!w{\w+}(=|\W))");

  std::string document("=You know about me.=");
  std::istringstream string_stream(document);

  Query query = reql(pattern);
  FStreamReader reader(string_stream);
  auto iter = query.finditer(&reader);

  std::vector<Span> actual_spans;
  std::vector<std::string> actual_strings;

  for (const auto& m : iter) {
    actual_spans.push_back(m.span("w"));
    actual_strings.push_back(m.group("w"));
  }

  std::vector<Span> expected_spans = {{1, 4}, {5, 9}, {10, 15}, {16, 18}};
  std::vector<std::string> expected_groups = {"You", "know", "about", "me"};

  REQUIRE(actual_spans == expected_spans);
  REQUIRE(actual_strings == expected_groups);
}

TEST_CASE("An exception is thrown when the circular buffer is not big enough") {
  std::string pattern = R"(!x{\d\d\d\d\d\d})";
  std::string document = "01234567";
  std::istringstream string_stream(document);

  Query query = reql(pattern, Flags::NONE, DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                     DEFAULT_MAX_DETERMINISTIC_STATES, 5);

  FStreamReader reader(string_stream);
  auto iter = query.finditer(&reader);

  auto match{iter.begin()};
  REQUIRE_THROWS_AS((*match).group(0), EvaluationException);
}

TEST_CASE("reql stream works when the filtering does not have enough buffer") {
  std::string pattern = R"(\W!x{\d+}\W)";
  std::string document = "-12-456---012345678901234567890-234-";
  std::istringstream string_stream(document);

  Query query = reql(pattern, Flags::NONE, DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                     DEFAULT_MAX_DETERMINISTIC_STATES, 8);

  FStreamReader reader(string_stream);
  auto iter = query.finditer(&reader);

  std::vector<Span> actual_spans;
  std::vector<std::string> actual_groups;

  for (const auto& m : iter) {
    actual_spans.push_back(m.span(0));
    try {
      actual_groups.push_back(m.group(0));
    } catch (EvaluationException& e) {
      actual_groups.emplace_back("");
    }
  }

  std::vector<Span> expected_spans = {{1, 3}, {4, 7}, {10, 31}, {32, 35}};
  std::vector<std::string> expected_groups = {"12", "456", "", "234"};

  REQUIRE(actual_spans == expected_spans);
  REQUIRE(actual_groups == expected_groups);
}

TEST_CASE("reql stream works when a match is the same size as the buffer") {
  std::string pattern = R"(!x{\d+})";
  std::string document = "-12345-";
  std::istringstream string_stream(document);

  Query query = reql(pattern, Flags::NONE, DEFAULT_MAX_MEMPOOL_DUPLICATIONS,
                     DEFAULT_MAX_DETERMINISTIC_STATES, 5);

  FStreamReader reader(string_stream);
  auto iter = query.finditer(&reader);

  std::set<Span> actual_spans;
  std::set<std::string> actual_groups;

  for (const auto& m : iter) {
    actual_spans.insert(m.span(0));
    try {
      actual_groups.insert(m.group(0));
    } catch (EvaluationException& e) {
      actual_groups.insert("");
    }
  }

  std::set<Span> expected_spans = {
      {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {2, 3}, {2, 4}, {2, 5},
      {2, 6}, {3, 4}, {3, 5}, {3, 6}, {4, 5}, {4, 6}, {5, 6},
  };
  // "12345" is not in the expected set because the algorithm needs an extra character at the end
  // and the buffer is full already
  std::set<std::string> expected_groups = {
      "1", "12", "123", "1234", "", "2", "23", "234", "2345", "3", "34", "345", "4", "45", "5",
  };

  REQUIRE(actual_spans == expected_spans);
  REQUIRE(actual_groups == expected_groups);
}

TEST_CASE("reql stream line by line a") {
  std::string pattern = R"(^!x{.{2,}})";
  std::string document = "012\n45\n6\n";
  std::istringstream string_stream(document);

  Query query = reql(pattern, Flags::LINE_BY_LINE);
  FStreamReader reader(string_stream);
  auto iter = query.finditer(&reader);

  std::set<Span> actual_spans;
  std::set<std::string> actual_groups;

  std::stringstream info;
  info << "Actual spans:\n";
  for (const auto& m : iter) {
    actual_spans.insert(m.span(0));
    info << m.span(0).first << " " << m.span(0).second << "\n";
    try {
      actual_groups.insert(m.group(0));
    } catch (EvaluationException& e) {
      actual_groups.insert("");
    }
  }

  std::set<Span> expected_spans = {{0, 2}, {0, 3}, {4, 6}};
  std::set<std::string> expected_groups = {"01", "012", "45"};

  INFO(info.str());
  REQUIRE(actual_spans == expected_spans);
  REQUIRE(actual_groups == expected_groups);
}

TEST_CASE("reql stream line by line b") {
  std::string pattern = R"(^!x{.{2,}})";
  std::string document = "0\n23\n567";
  std::istringstream string_stream(document);

  Query query = reql(pattern, Flags::LINE_BY_LINE);
  FStreamReader reader(string_stream);
  auto iter = query.finditer(&reader);

  std::set<Span> actual_spans;
  std::set<std::string> actual_groups;

  std::stringstream info;
  info << "Actual spans:\n";
  for (const auto& m : iter) {
    actual_spans.insert(m.span(0));
    info << m.span(0).first << " " << m.span(0).second << "\n";
    try {
      actual_groups.insert(m.group(0));
    } catch (EvaluationException& e) {
      actual_groups.insert("");
    }
  }

  std::set<Span> expected_spans = {{2, 4}, {5, 7}, {5, 8}};
  std::set<std::string> expected_groups = {"23", "56", "567"};

  INFO(info.str());
  REQUIRE(actual_spans == expected_spans);
  REQUIRE(actual_groups == expected_groups);
}

}  // namespace REmatch::testing
