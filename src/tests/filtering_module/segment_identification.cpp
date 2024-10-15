#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "parsing/parser.hpp"
#include "string_view"
#include "filtering_module/segment_identificator.hpp"
#include "evaluation/document.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {
TEST_CASE("The regex 'a' will return the span 0,5 on the document 'aaaaa'") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  auto search_dfa = SearchDFA(logical_va);
  auto document = std::make_shared<Document>("aaaaa");
  auto segment_identificator = SegmentIdentificator(search_dfa, document);
  std::unique_ptr<Span> output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  REQUIRE(output_span->first == 0);
  REQUIRE(output_span->second == 5);
  REQUIRE(segment_identificator.next() == nullptr);
}

TEST_CASE("The regex 'a' will return the spans: 0,1 2,3 4,5 \
           on the document 'ababa'") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  auto search_dfa = SearchDFA(logical_va);
  auto document = std::make_shared<Document>("ababa");
  auto segment_identificator = SegmentIdentificator(search_dfa, document);
  std::unique_ptr<Span> output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  REQUIRE(output_span->first == 0);
  REQUIRE(output_span->second == 1);
  output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  REQUIRE(output_span->first == 2);
  REQUIRE(output_span->second == 3);
  output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  REQUIRE(output_span->first == 4);
  REQUIRE(output_span->second == 5);
  REQUIRE(segment_identificator.next() == nullptr);
}

TEST_CASE("The regex 'αβ' will return the span 6,10 \
           on the document 'δεζαβab' in segmentation") {
  Parser parser = Parser("αβ");
  LogicalVA logical_va = parser.get_logical_va();
  auto document = std::make_shared<Document>("δεζαβab");
  auto search_dfa = SearchDFA(logical_va);
  auto segment_identificator = SegmentIdentificator(search_dfa, document);
  std::unique_ptr<Span> output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  INFO("first: " << output_span->first << " second: " << output_span->second);
  REQUIRE(output_span->first == 6);
  REQUIRE(output_span->second == 10);
  REQUIRE(segment_identificator.next() == nullptr);
}

TEST_CASE("The regex '[αβ]' will return the spans: 6,10 \
           on the document 'δεζαβab' in segmentation") {
  Parser parser = Parser("αβ");
  LogicalVA logical_va = parser.get_logical_va();
  auto document = std::make_shared<Document>("δεζαβab");
  auto search_dfa = SearchDFA(logical_va);
  auto segment_identificator = SegmentIdentificator(search_dfa, document);
  std::unique_ptr<Span> output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  INFO("first: " << output_span->first << " second: " << output_span->second);
  REQUIRE(output_span->first == 6);
  REQUIRE(output_span->second == 10);
  REQUIRE(segment_identificator.next() == nullptr);
}

TEST_CASE("The regex '[α-ε]' will return the spans: <0,5> <6,10> \
           on the document 'δεζαβab' in segmentation") {
  Parser parser = Parser("[α-ε]");
  LogicalVA logical_va = parser.get_logical_va();
  auto document = std::make_shared<Document>("δεζαβab");
  auto search_dfa = SearchDFA(logical_va);
  auto segment_identificator = SegmentIdentificator(search_dfa, document);
  std::unique_ptr<Span> output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  INFO("first: " << output_span->first << " second: " << output_span->second);
  REQUIRE(output_span->first == 0);
  REQUIRE(output_span->second == 2);
  output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  INFO("first: " << output_span->first << " second: " << output_span->second);
  REQUIRE(output_span->first == 2);
  REQUIRE(output_span->second == 4);
  output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  INFO("first: " << output_span->first << " second: " << output_span->second);
  REQUIRE(output_span->first == 6);
  REQUIRE(output_span->second == 8);
  output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  INFO("first: " << output_span->first << " second: " << output_span->second);
  REQUIRE(output_span->first == 8);
  REQUIRE(output_span->second == 10);
  REQUIRE(segment_identificator.next() == nullptr);
}

TEST_CASE("The regex .α returns 0,8 on the document: δεζαβab") {
  /**
   * The reason why the output it 0,8 is because there is no ends before
   * reaching alpha, as it enters the 2 char utf-8 branch, continues and
   * while doing that reenters that branch until it is finished.
   */
  Parser parser = Parser(".α");
  LogicalVA logical_va = parser.get_logical_va();
  auto document = std::make_shared<Document>("δεζαβab");
  auto search_dfa = SearchDFA(logical_va);
  auto segment_identificator = SegmentIdentificator(search_dfa, document);
  std::unique_ptr<Span>  output_span = segment_identificator.next();
  shift_span(output_span);
  REQUIRE(output_span != nullptr);
  REQUIRE(output_span->first == 0);
  REQUIRE(output_span->second == 8);
}

}  // namespace REmatch::testing
