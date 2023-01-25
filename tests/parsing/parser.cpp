#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "parsing/parser.hpp"

namespace rematch::testing {
TEST_CASE("the regex 'a' is parsed correctly into a LogicalVA.") {
  Parser parser = Parser("a");
  rematch::LogicalVA va = parser.get_logical_va();
  INFO(va.initial_state()->filters.back()->charclass);
  REQUIRE(va.states.size() == 2);
  REQUIRE(va.initial_state()->filters.size() == 1);
  REQUIRE(va.initial_state()->captures.empty());
  REQUIRE(va.initial_state()->filters.back()->charclass.contains('a'));
  REQUIRE(va.initial_state()->filters.back()->next == va.accepting_state());
  REQUIRE(va.accepting_state()->backward_filters_.size() == 1);
  REQUIRE(va.accepting_state()->accepting());
  REQUIRE(va.accepting_state()->backward_filters_.back()->next == va.accepting_state());
}

TEST_CASE("the regex '!x{a}' is parsed correctly into a LogicalVA.") {
  Parser parser = Parser("!x{a}");
  rematch::LogicalVA va = parser.get_logical_va();
  // initial - !x{ -> 2 - a -> 3 - !x} -> final
  REQUIRE(va.states.size() == 4);
  REQUIRE(va.initial_state()->captures.size() == 1);
  REQUIRE(va.initial_state()->captures.front()->next->filters.size() == 1);
  LogicalVAState *second_state = va.initial_state() -> captures.front()->next;
  REQUIRE(second_state->filters.back()->charclass.contains('a'));
  LogicalVAState *third_state = second_state->filters.back()->next;
  LogicalVAState *fourth_state = third_state->captures.back()->next;
  REQUIRE(fourth_state->accepting());
  REQUIRE((va.initial_state()->captures.back()->code << 1) != 0);
  REQUIRE((va.initial_state()->captures.back()->code << 1) == third_state->captures.back()->code);
}

}  // namespace rematch::testing
