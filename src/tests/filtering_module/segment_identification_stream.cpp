#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../tests_utils/tests_utils.hpp"
#include "evaluation/document.hpp"
#include "filtering_module/segment_identificator.hpp"
#include "filtering_module/segment_identificator_stream.hpp"
#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE(
    "The regex 'a' will return the span 0,5 on the document 'aaaaa' using "
    "stream") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  auto search_dfa = std::make_unique<SearchDFA>(logical_va);
  std::istringstream doc("aaaaa");
  FStreamReader reader(doc);
  auto stream = std::make_shared<Stream>(&reader);
  auto segment_identificator = SegmentIdentificatorStream(std::move(search_dfa), stream);
  auto segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  REQUIRE(segment_result.first->first == 0);
  REQUIRE(segment_result.first->second == 5);
  REQUIRE(segment_identificator.next().second == NoSegmentFound);
}

TEST_CASE(
    "The regex 'a' will return the spans: 0,1 2,3 4,5 \
           on the document 'ababa' using stream") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  auto search_dfa = std::make_unique<SearchDFA>(logical_va);
  std::istringstream doc("ababa");
  FStreamReader reader(doc);
  auto stream = std::make_shared<Stream>(&reader);
  auto segment_identificator = SegmentIdentificatorStream(std::move(search_dfa), stream);
  auto segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  REQUIRE(segment_result.first->first == 0);
  REQUIRE(segment_result.first->second == 1);
  segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  REQUIRE(segment_result.first->first == 2);
  REQUIRE(segment_result.first->second == 3);
  segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  REQUIRE(segment_result.first->first == 4);
  REQUIRE(segment_result.first->second == 5);
  REQUIRE(segment_identificator.next().second == NoSegmentFound);
}

TEST_CASE(
    "The regex 'αβ' will return the span 6,10 \
           on the document 'δεζαβab' in segmentation using stream") {
  Parser parser = Parser("αβ");
  LogicalVA logical_va = parser.get_logical_va();
  std::istringstream doc("δεζαβab");
  FStreamReader reader(doc);
  auto stream = std::make_shared<Stream>(&reader);
  auto search_dfa = std::make_unique<SearchDFA>(logical_va);
  auto segment_identificator = SegmentIdentificatorStream(std::move(search_dfa), stream);
  auto segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  INFO("first: " << segment_result.first->first << " second: " << segment_result.first->second);
  REQUIRE(segment_result.first->first == 6);
  REQUIRE(segment_result.first->second == 10);
  REQUIRE(segment_identificator.next().second == NoSegmentFound);
}

TEST_CASE(
    "The regex '[αβ]' will return the spans: 6,10 \
           on the document 'δεζαβab' in segmentation using stream") {
  Parser parser = Parser("αβ");
  LogicalVA logical_va = parser.get_logical_va();
  std::istringstream doc("δεζαβab");
  FStreamReader reader(doc);
  auto stream = std::make_shared<Stream>(&reader);
  auto search_dfa = std::make_unique<SearchDFA>(logical_va);
  auto segment_identificator = SegmentIdentificatorStream(std::move(search_dfa), stream);
  auto segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  INFO("first: " << segment_result.first->first << " second: " << segment_result.first->second);
  REQUIRE(segment_result.first->first == 6);
  REQUIRE(segment_result.first->second == 10);
  REQUIRE(segment_identificator.next().second == NoSegmentFound);
}

// TODO: this test fails in the commented lines
TEST_CASE(
    "The regex '[α-ε]' will return the spans: <0,5> <6,10> \
           on the document 'δεζαβab' in segmentation using stream") {
  Parser parser = Parser("[α-ε]");
  LogicalVA logical_va = parser.get_logical_va();
  std::istringstream doc("δεζαβab");  // fe ce b4 ce b5 ce fe ce b4 ce b5 ce b6 ce b1 ce b2 a b
  FStreamReader reader(doc);
  auto stream = std::make_shared<Stream>(&reader);
  auto search_dfa = std::make_unique<SearchDFA>(logical_va);
  auto segment_identificator = SegmentIdentificatorStream(std::move(search_dfa), stream);
  auto segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  INFO("first: " << segment_result.first->first << " second: " << segment_result.first->second);
  REQUIRE(segment_result.first->first == 0);
  REQUIRE(segment_result.first->second == 2);
  segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  INFO("first: " << segment_result.first->first << " second: " << segment_result.first->second);
  REQUIRE(segment_result.first->first == 2);
  REQUIRE(segment_result.first->second == 4);
  segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  INFO("first: " << segment_result.first->first << " second: " << segment_result.first->second);
  REQUIRE(segment_result.first->first == 6);
  REQUIRE(segment_result.first->second == 8);
  segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  INFO("first: " << segment_result.first->first << " second: " << segment_result.first->second);
  REQUIRE(segment_result.first->first == 8);
  REQUIRE(segment_result.first->second == 10);
  REQUIRE(segment_identificator.next().second == NoSegmentFound);
}

TEST_CASE("The regex .α returns 0,8 on the document: δεζαβab using stream") {
  /**
   * The reason why the output it 0,8 is because there is no ends before
   * reaching alpha, as it enters the 2 char utf-8 branch, continues and
   * while doing that reenters that branch until it is finished.
   */
  Parser parser = Parser(".α");
  LogicalVA logical_va = parser.get_logical_va();
  std::istringstream doc("δεζαβab");
  FStreamReader reader(doc);
  auto stream = std::make_shared<Stream>(&reader);
  auto search_dfa = std::make_unique<SearchDFA>(logical_va);
  auto segment_identificator = SegmentIdentificatorStream(std::move(search_dfa), stream);
  auto segment_result = segment_identificator.next();
  REQUIRE(segment_result.second != NoSegmentFound);
  REQUIRE(segment_result.first->first == 0);
  REQUIRE(segment_result.first->second == 8);
}

}  // namespace REmatch::testing
