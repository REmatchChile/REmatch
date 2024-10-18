#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "parsing/parser.hpp"
#include "string_view"
#include "filtering_module/search_variable_set_automaton/dfa/search_dfa.hpp"

namespace REmatch::testing {

TEST_CASE("search_dfa with 'a' is accepting after any 'a' transition") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  SearchDFA search_dfa = SearchDFA(logical_va);
  INFO("initial state: " << search_dfa.get_initial_state()->get_id());
  SearchDFAState *current_state = search_dfa.next_state('a');
  // First new DFA state created
  REQUIRE(current_state->accepting());
  REQUIRE(current_state->ends());
  INFO("Current state: " << current_state->get_id());
  current_state = search_dfa.next_state('b');
  REQUIRE(current_state->ends());
  current_state = search_dfa.next_state('a');
  // third new DFA state created (has ends)
  REQUIRE(current_state->ends());
  REQUIRE(current_state->accepting());
  current_state = search_dfa.next_state('a');
  REQUIRE(current_state->ends());
  REQUIRE(current_state->accepting());
  REQUIRE(search_dfa.states.size() == 3);
}

TEST_CASE("search_dfa with 'α' is accepting after '0xce 0xbe' transitions") {
  Parser parser = Parser("α");
  LogicalVA logical_va = parser.get_logical_va();
  SearchDFA search_dfa = SearchDFA(logical_va);
  INFO("initial state: " << search_dfa.get_initial_state()->get_id());
  SearchDFAState *current_state = search_dfa.next_state((char) 0xce);
  REQUIRE(current_state != nullptr);
  // First new DFA state created
  REQUIRE(!current_state->accepting());
  REQUIRE(current_state->ends());
  INFO("Current state: " << current_state->get_id());
  current_state = search_dfa.next_state((char) 0xb1);
  REQUIRE(current_state->accepting());
  REQUIRE(!current_state->ends());
  REQUIRE(search_dfa.states.size() == 3);
}

}  // namespace REmatch::testing
