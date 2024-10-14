#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "parsing/parser.hpp"
#include "string_view"
#include "filtering_module/search_variable_set_automaton/nfa/search_nfa.hpp"

namespace REmatch::testing {

void conditions_for_basic_a_search_nfa_automaton(SearchNFA &search_nfa) {
  REQUIRE(search_nfa.size() == 2);
  REQUIRE(search_nfa.initial_state()->initial());
  REQUIRE(search_nfa.initial_state()->filters.size() == 1);
  REQUIRE(search_nfa.initial_state()->filters.back()->charclass.contains('a'));;
  REQUIRE(search_nfa.initial_state()->filters.back()->next == search_nfa.accepting_state());
  REQUIRE(search_nfa.accepting_state()->accepting());
}

TEST_CASE("search_nfa with 'a' parsed correctly") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  SearchNFA search_nfa = SearchNFA(logical_va);
  conditions_for_basic_a_search_nfa_automaton(search_nfa);
}

TEST_CASE("search_nfa with '!x{a}' parsed correctly") {
  Parser parser = Parser("!x{a}");
  LogicalVA logical_va = parser.get_logical_va();
  SearchNFA search_nfa = SearchNFA(logical_va);
  conditions_for_basic_a_search_nfa_automaton(search_nfa);
}

TEST_CASE("search_nfa with 'α' parsed correctly") {
  Parser parser = Parser("α");
  LogicalVA logical_va = parser.get_logical_va();
  SearchNFA search_nfa = SearchNFA(logical_va);
  REQUIRE(search_nfa.size() == 3);
  REQUIRE(search_nfa.initial_state()->initial());
  REQUIRE(search_nfa.initial_state()->filters.size() == 1);
  REQUIRE(search_nfa.initial_state()->filters.back()->charclass.contains((char) 0xce));;
  SearchNFAState* second_state = search_nfa.initial_state()->filters.back()->next;
  REQUIRE(second_state->filters.size() == 1);
  REQUIRE(second_state->filters.back()->charclass.contains((char) 0xb1));;
}


}  // namespace REmatch::testing
