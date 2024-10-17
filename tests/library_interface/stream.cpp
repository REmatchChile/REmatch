#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "exceptions/anchor_inside_capture_exception.hpp"
#include "parsing/parser.hpp"

#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"
#include "library_interface/query_stream.hpp"
#include "library_interface/reader.hpp"

namespace rematch::testing {

TEST_CASE("something something") {
  std::cout << "01" << std::endl;
  std::string pattern = "a";
  Parser parser = Parser(pattern);
  LogicalVA logical_va = parser.get_logical_va();
  auto search_dfa = SearchDFA(logical_va);

  std::string document("aaaaa");
  std::istringstream stream(document);

  auto reader = std::make_shared<FileStreamReader>(stream);
  SegmentIdentificatorStream segment_identificator(search_dfa, reader);

  auto span = segment_identificator.next();
  if (span != nullptr) {
    std::cout << span->first << std::endl;
    std::cout << span->second << std::endl;
  }
  span = segment_identificator.next();
  if (span == nullptr) {
    std::cout << "great" << std::endl;
  }
  std::cout << "01\n" << std::endl;
}

TEST_CASE("something") {
  std::cout << "02" << std::endl;
  Parser parser = Parser("a");
  rematch::LogicalVA logical_va = parser.get_logical_va();
  auto search_dfa = SearchDFA(logical_va);

  std::string document = "ababa";
  std::istringstream stream(document);
  //  FileStreamReader reader(stream);

  auto reader = std::make_shared<FileStreamReader>(stream);
  SegmentIdentificatorStream segment_identificator(search_dfa, reader);

  std::unique_ptr<Span> span = segment_identificator.next();
  REQUIRE(span != nullptr);
  REQUIRE(span->first == 0);
  REQUIRE(span->second == 1);
  span = segment_identificator.next();
  REQUIRE(span != nullptr);
  REQUIRE(span->first == 2);
  REQUIRE(span->second == 3);
  span = segment_identificator.next();
  REQUIRE(span != nullptr);
  REQUIRE(span->first == 4);
  REQUIRE(span->second == 5);
  REQUIRE(segment_identificator.next() == nullptr);
  std::cout << "02\n" << std::endl;
}

TEST_CASE("stream big document") {
  std::cout << "03" << std::endl;
  Parser parser = Parser("a");
  rematch::LogicalVA logical_va = parser.get_logical_va();
  auto search_dfa = SearchDFA(logical_va);

  std::string document =
      "aaa aaa bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbaaaaabbbbbbbbbbbbb aaaaaa "
      "bbbbbbbbbbbbbbbbbbbbbb";
  std::istringstream stream(document);
  // FileStreamReader reader(stream);

  auto fsreader = std::make_shared<FileStreamReader>(stream);
  auto segment_identificator = SegmentIdentificatorStream(search_dfa, fsreader);

  std::unique_ptr<Span> span = segment_identificator.next();

  while (span != nullptr) {
    std::cout << span->first << " " << span->second << std::endl;
    span = segment_identificator.next();
    if (span->first > 130 || span->second > 130)
      break;
  }
  std::cout << "03\n" << std::endl;
}

TEST_CASE("rewind") {
  std::string document = "hello";
  std::istringstream stream(document);

  std::cout << "REWIND" << std::endl;

  char buf[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  stream.read(buf, 10);

  std::cout << buf << std::endl;

  std::basic_istream<char>::pos_type pos = 2;

  stream.clear();
  stream.seekg(pos);

  stream.read(buf, 10);

  std::cout << buf << std::endl;
  std::cout << "REWIND END" << std::endl;
}

TEST_CASE("query stream 0") {
  std::string pattern = "!x{ab}";
  std::istringstream stream("abab");
  auto reader = std::make_shared<FileStreamReader>(stream);

  auto query = QueryStream(pattern);
  auto match = query.find(reader);

  REQUIRE(match->span("x") == Span(0, 2));
}

TEST_CASE("query stream 1") {
  std::string pattern = "!x{55555|55556666|444444555555555}";
  std::istringstream stream(
      "000000000011111111112222222222333333333344444444445555555555666666666677"
      "77777777888888888899999999");
  auto reader = std::make_shared<FileStreamReader>(stream);

  auto query = QueryStream(pattern);

  auto iterator = query.finditer(reader);

  auto match = iterator->next();

  while (match != nullptr) {
    std::cout << "Span: " << match->span(0) << std::endl;
    auto match = iterator->next();
  }
}

}  // namespace rematch::testing
