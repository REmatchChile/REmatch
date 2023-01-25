#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "parsing/parser.hpp"
#include "string_view"
#include "segment_identification/search_variable_set_automaton/dfa/search_dfa.hpp"

namespace rematch::testing {

TEST_CASE("search_dfa with 'a' is accepting after any 'a' transition") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  SearchDFA search_dfa = SearchDFA(logical_va);
  INFO("initial state: " << search_dfa.get_initial_state()->get_id());
  SearchDFAState *current_state = search_dfa.next_state('a');
  REQUIRE(current_state->accepting());
  INFO("Current state: " << current_state->get_id());
  current_state = search_dfa.next_state('b');
  REQUIRE(current_state->ends());
  current_state = search_dfa.next_state('a');
  REQUIRE(current_state->accepting());
  current_state = search_dfa.next_state('a');
  REQUIRE(current_state->accepting());
  REQUIRE(search_dfa.states.size() == 2);
}

}  // namespace rematch::testing
