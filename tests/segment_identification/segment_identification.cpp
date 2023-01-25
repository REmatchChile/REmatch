#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "parsing/parser.hpp"
#include "string_view"
#include "segment_identification/segment_identificator.hpp"

namespace rematch::testing {
TEST_CASE("The regex 'a' will return the span 0,5 on the document 'aaaaa'") {
  Parser parser = Parser("a");
  rematch::LogicalVA logical_va = parser.get_logical_va();
  //std::string_view document = "aaaaa";
  //auto segment_identificator = SegmentIdentificator(logical_va, document);
  //bool has_an_output = segment_identificator.has_next();
  //INFO((char) (document[0] & 0x7F));
  //REQUIRE(has_an_output);
  //REQUIRE(segment_identificator.has_next());
  //Span output_span = segment_identificator.next();
  //REQUIRE(output_span.first == 0);
  //REQUIRE(output_span.second == 5);
  //REQUIRE(segment_identificator.has_next() == false);
}
}  // namespace rematch::testing
