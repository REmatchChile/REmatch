#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "parsing/parser.hpp"
#include "string_view"
#include "filtering_module/segment_identificator.hpp"

namespace rematch::testing {
TEST_CASE("The regex 'a' will return the span 0,5 on the document 'aaaaa'") {
  Parser parser = Parser("a");
  rematch::LogicalVA logical_va = parser.get_logical_va();
  std::string_view document = "aaaaa";
  auto segment_identificator = SegmentIdentificator(logical_va, document);
  REQUIRE(segment_identificator.has_next());
  Span output_span = segment_identificator.next();
  REQUIRE(output_span.first == 0);
  REQUIRE(output_span.second == 5);
  REQUIRE(segment_identificator.has_next() == false);
}

TEST_CASE("The regex 'a' will return the spans: 0,1 2,3 4,5 \
           on the document 'ababa'") {
  Parser parser = Parser("a");
  rematch::LogicalVA logical_va = parser.get_logical_va();
  std::string_view document = "ababa";
  auto segment_identificator = SegmentIdentificator(logical_va, document);
  REQUIRE(segment_identificator.has_next());
  Span output_span = segment_identificator.next();
  REQUIRE(output_span.first == 0);
  REQUIRE(output_span.second == 1);
  REQUIRE(segment_identificator.has_next());
  output_span = segment_identificator.next();
  REQUIRE(output_span.first == 2);
  REQUIRE(output_span.second == 3);
  REQUIRE(segment_identificator.has_next());
  output_span = segment_identificator.next();
  REQUIRE(output_span.first == 4);
  REQUIRE(output_span.second == 5);
  REQUIRE(segment_identificator.has_next() == false);
}

TEST_CASE("The regex 'αβ' will return the span 6,10 \
           on the document 'δεζαβab' in segmentation") {
  Parser parser = Parser("αβ");
  rematch::LogicalVA logical_va = parser.get_logical_va();
  std::string_view document = "δεζαβab";
  auto segment_identificator = SegmentIdentificator(logical_va, document);
  REQUIRE(segment_identificator.has_next());
  Span output_span = segment_identificator.next();
  INFO("first: " << output_span.first << " second: " << output_span.second);
  REQUIRE(output_span.first == 6);
  REQUIRE(output_span.second == 10);
  REQUIRE(segment_identificator.has_next() == false);
}

TEST_CASE("The regex '[αβ]' will return the spans: 6,10 \
           on the document 'δεζαβab' in segmentation") {
  Parser parser = Parser("αβ");
  rematch::LogicalVA logical_va = parser.get_logical_va();
  std::string_view document = "δεζαβab";
  auto segment_identificator = SegmentIdentificator(logical_va, document);
  REQUIRE(segment_identificator.has_next());
  Span output_span = segment_identificator.next();
  INFO("first: " << output_span.first << " second: " << output_span.second);
  REQUIRE(output_span.first == 6);
  REQUIRE(output_span.second == 10);
  REQUIRE(segment_identificator.has_next() == false);
}

TEST_CASE("The regex '[α-ε]' will return the spans: <0,5> <6,10> \
           on the document 'δεζαβab' in segmentation") {
  Parser parser = Parser("[α-ε]");
  rematch::LogicalVA logical_va = parser.get_logical_va();
  std::string_view document = "δεζαβab";
  auto segment_identificator = SegmentIdentificator(logical_va, document);
  REQUIRE(segment_identificator.has_next());
  Span output_span = segment_identificator.next();
  INFO("first: " << output_span.first << " second: " << output_span.second);
  REQUIRE(output_span.first == 0);
  REQUIRE(output_span.second == 2);
  REQUIRE(segment_identificator.has_next());
  output_span = segment_identificator.next();
  INFO("first: " << output_span.first << " second: " << output_span.second);
  REQUIRE(output_span.first == 2);
  REQUIRE(output_span.second == 4);
  REQUIRE(segment_identificator.has_next());
  output_span = segment_identificator.next();
  INFO("first: " << output_span.first << " second: " << output_span.second);
  REQUIRE(output_span.first == 6);
  REQUIRE(output_span.second == 8);
  REQUIRE(segment_identificator.has_next());
  output_span = segment_identificator.next();
  INFO("first: " << output_span.first << " second: " << output_span.second);
  REQUIRE(output_span.first == 8);
  REQUIRE(output_span.second == 10);
  REQUIRE(segment_identificator.has_next() == false);
}


}  // namespace rematch::testing
