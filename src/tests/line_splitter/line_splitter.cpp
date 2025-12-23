#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "mediator/line_splitter.hpp"

namespace REmatch::testing {

TEST_CASE("Line splitter works correctly a") {
  std::string document = "01234\n678\n012345";

  auto line_splitter = LineSplitterStr(std::make_shared<Document>(document));

  std::vector<Span> expected_lines = {{0, 5}, {6, 9}, {10, 16}};
  std::vector<Span> actual_lines;

  std::stringstream info;
  info << "Actual lines:\n";
  while (auto line = line_splitter.get_line()) {
    actual_lines.push_back(*line);
    info << line->first << " " << line->second << "\n";
  }

  INFO(info.str());
  REQUIRE(expected_lines == actual_lines);
}

TEST_CASE("Line splitter works correctly b") {
  std::string document = "01234\n678\n012345\n";

  auto line_splitter = LineSplitterStr(std::make_shared<Document>(document));

  std::vector<Span> expected_lines = {{0, 5}, {6, 9}, {10, 16}, {17, 17}};
  std::vector<Span> actual_lines;

  std::stringstream info;
  info << "Actual lines:\n";
  while (auto line = line_splitter.get_line()) {
    actual_lines.push_back(*line);
    info << line->first << " " << line->second << "\n";
  }

  INFO(info.str());
  REQUIRE(expected_lines == actual_lines);
}

TEST_CASE("Line splitter works correctly c") {
  std::string document = "\n123\n5";

  auto line_splitter = LineSplitterStr(std::make_shared<Document>(document));

  std::vector<Span> expected_lines = {{0, 0}, {1, 4}, {5, 6}};
  std::vector<Span> actual_lines;

  std::stringstream info;
  info << "Actual lines:\n";
  while (auto line = line_splitter.get_line()) {
    actual_lines.push_back(*line);
    info << line->first << " " << line->second << "\n";
  }

  INFO(info.str());
  REQUIRE(expected_lines == actual_lines);
}

TEST_CASE("Line splitter stream works correctly a") {
  std::stringstream document("01234\n678\n012345");

  auto reader = std::make_unique<FStreamReader>(document);
  auto stream = std::make_shared<Stream>(reader.get());

  auto line_splitter = LineSplitterStream(stream);

  std::vector<Span> expected_lines = {{0, 5}, {6, 9}, {10, 16}};
  std::vector<Span> actual_lines;

  std::stringstream info;
  info << "Actual lines:\n";
  while (auto line = line_splitter.get_line()) {
    actual_lines.push_back(*line);
    info << line->first << " " << line->second << "\n";
  }

  INFO(info.str());
  REQUIRE(expected_lines == actual_lines);
}

TEST_CASE("Line splitter stream works correctly b") {
  std::stringstream document("01234\n678\n012345\n");

  auto reader = std::make_unique<FStreamReader>(document);
  auto stream = std::make_shared<Stream>(reader.get());

  auto line_splitter = LineSplitterStream(stream);

  std::vector<Span> expected_lines = {{0, 5}, {6, 9}, {10, 16}, {17, 17}};
  std::vector<Span> actual_lines;

  std::stringstream info;
  info << "Actual lines:\n";
  while (auto line = line_splitter.get_line()) {
    actual_lines.push_back(*line);
    info << line->first << " " << line->second << "\n";
  }

  INFO(info.str());
  REQUIRE(expected_lines == actual_lines);
}

TEST_CASE("Line splitter stream works correctly c") {
  std::stringstream document("\n123\n5");

  auto reader = std::make_unique<FStreamReader>(document);
  auto stream = std::make_shared<Stream>(reader.get());

  auto line_splitter = LineSplitterStream(stream);

  std::vector<Span> expected_lines = {{0, 0}, {1, 4}, {5, 6}};
  std::vector<Span> actual_lines;

  std::stringstream info;
  info << "Actual lines:\n";
  while (auto line = line_splitter.get_line()) {
    actual_lines.push_back(*line);
    info << line->first << " " << line->second << "\n";
  }

  INFO(info.str());
  REQUIRE(expected_lines == actual_lines);
}

}  // namespace REmatch::testing
