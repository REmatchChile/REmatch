#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "parsing/parser.hpp"

namespace REmatch::testing {
TEST_CASE("Removing captures of '!x{a}' is correct.") {
  Parser parser = Parser("!x{a}");
  LogicalVA va = parser.get_logical_va();
  va.remove_captures();

  REQUIRE(va.initial_state()->epsilons.size() == 1);
  REQUIRE(va.initial_state()->filters.empty());
  REQUIRE(va.initial_state()->captures.empty());

  LogicalVAState *second_state = va.initial_state() -> epsilons.front()->next;
  REQUIRE(second_state->epsilons.empty());
  REQUIRE(second_state->filters.size() == 1);
  REQUIRE(second_state->captures.empty());

  LogicalVAState *third_state = second_state->filters.back()->next;
  REQUIRE(third_state->epsilons.size() == 1);
  REQUIRE(third_state->filters.empty());
  REQUIRE(third_state->captures.empty());

  LogicalVAState *fourth_state = third_state->epsilons.back()->next;
  REQUIRE(fourth_state->captures.empty());
  REQUIRE(fourth_state->filters.empty());
  REQUIRE(fourth_state->epsilons.empty());
  REQUIRE(fourth_state->accepting());
}

TEST_CASE("Removing epsilons of '!x{a}' after remove_captures is correct.") {
  Parser parser = Parser("!x{a}");
  LogicalVA va = parser.get_logical_va();
  va.remove_captures();
  va.remove_epsilon();
  REQUIRE(va.initial_state()->captures.empty());
  REQUIRE(va.initial_state()->epsilons.empty());
  REQUIRE(va.initial_state()->filters.size() == 2);
  LogicalVAState *second_state = va.initial_state()->filters.back()->next;
  LogicalVAState *third_state = va.initial_state()->filters.front()->next;
  bool one_is_accepting = second_state->accepting() || third_state->accepting();
  REQUIRE(one_is_accepting);
}

TEST_CASE("trimming !x{a} after removing captures and epsilons gives \
           two states.") {
  Parser parser = Parser("!x{a}");
  LogicalVA va = parser.get_logical_va();
  va.remove_captures();
  va.remove_epsilon();
  va.trim();
  REQUIRE(va.states.size() == 2);
  INFO(va.initial_state()->id);
  INFO(va.accepting_state()->id);
  INFO(va.initial_state()->filters.front()->next->id);
  INFO(va.initial_state()->filters.back()->next->id);
  REQUIRE(va.initial_state()->filters.size() == 1);
  REQUIRE(va.initial_state()->filters.back()->next == va.accepting_state());
}

TEST_CASE("Relabeling the trimmed '!x{a}' after removing captures \
           and epsilons is correct.") {
  Parser parser = Parser("!x{a}");
  LogicalVA va = parser.get_logical_va();
  va.remove_captures();
  va.remove_epsilon();
  va.trim();
  va.relabel_states();
  INFO(va.states.size());
  LogicalVAState *second_state = va.initial_state()->filters.back()->next;
  REQUIRE(va.initial_state()->id == 0);
  REQUIRE(second_state->id == 1);
}

TEST_CASE("useless anchors in 'a^' are removed correctly") {
  Parser parser = Parser("a^");
  LogicalVA va = parser.get_logical_va();
  INFO(va.states.size());

  va.remove_useless_anchors();

  LogicalVAState* accepting_state = va.accepting_state();
  LogicalVAState* initial_state = va.initial_state();
  REQUIRE(accepting_state->backward_anchors_.size() == 0);
  REQUIRE(initial_state->filters.size() == 1);
}

TEST_CASE("useless anchors in '(a|$)a' are removed correctly") {
  Parser parser = Parser("(a|$)a");
  LogicalVA va = parser.get_logical_va();
  INFO(va.states.size());

  va.remove_useless_anchors();

  LogicalVAState* initial_state = va.initial_state();
  LogicalVAState* accepting_state = va.accepting_state();

  auto epsilons_it = initial_state->epsilons.begin();
  LogicalVAState* second_state = (*epsilons_it)->next;
  LogicalVAState* third_state = (*epsilons_it++)->next;

  REQUIRE(second_state->filters.size() == 1);
  REQUIRE(third_state->anchors.size() == 0);

  REQUIRE(accepting_state->backward_filters_.size() == 1);
}

TEST_CASE("remove anchors in 'a^' after removing epsilons is correct") {
  Parser parser = Parser("(a|$)a");
  LogicalVA va = parser.get_logical_va();
  INFO(va.states.size());

  va.remove_epsilon();
  va.remove_useless_anchors();

  for (auto &state: va.states)
    REQUIRE(state->epsilons.size() == 0);
}

TEST_CASE("remove anchors in '(a|$)a' after removing epsilons is correct") {
  Parser parser = Parser("(a|$)a");
  LogicalVA va = parser.get_logical_va();
  INFO(va.states.size());

  va.remove_epsilon();
  va.remove_useless_anchors();

  for (auto &state: va.states)
    REQUIRE(state->epsilons.size() == 0);
}

TEST_CASE("trimming 'a^' after removing anchors is correct") {
  Parser parser = Parser("a^");
  LogicalVA va = parser.get_logical_va();
  INFO(va.states.size());

  va.remove_epsilon();
  va.remove_useless_anchors();
  va.trim();

  // the initial state and the accepting state remain
  REQUIRE(va.states.size() == 2);
}

TEST_CASE("trimming '(a|$)a' after removing anchors is correct") {
  Parser parser = Parser("(a|$)a");
  LogicalVA va = parser.get_logical_va();
  INFO(va.states.size());

  va.remove_epsilon();
  va.remove_useless_anchors();
  va.trim();

  REQUIRE(va.states.size() == 3);
}

TEST_CASE("resulting lva '(a$)*' has useful anchor transitions") {
  Parser parser = Parser("(a$)*");
  LogicalVA va = parser.get_logical_va();
  INFO(va.states.size());

  va.remove_epsilon();
  va.remove_useless_anchors();
  va.trim();

  INFO(va.states.size());

  REQUIRE(va.has_useful_anchors());
}

TEST_CASE("resulting lva 'a($|^)' has useful anchor transitions") {
  Parser parser = Parser("a($|^)");
  LogicalVA va = parser.get_logical_va();
  INFO(va.states.size());

  va.remove_epsilon();
  va.remove_useless_anchors();
  va.trim();

  REQUIRE(va.has_useful_anchors());
}

TEST_CASE("accepting state is not reachable in '$a'") {
  Parser parser = Parser("$a");
  LogicalVA va = parser.get_logical_va();

  va.remove_epsilon();
  va.remove_useless_anchors();
  va.trim();

  REQUIRE(!va.is_accepting_state_reachable());
}

TEST_CASE("accepting state is not reachable in '[a-z]^'") {
  Parser parser = Parser("[a-z]^");
  LogicalVA va = parser.get_logical_va();

  va.remove_epsilon();
  va.remove_useless_anchors();
  va.trim();

  REQUIRE(!va.is_accepting_state_reachable());
}

TEST_CASE("accepting state is reachable in 'b(a^)*'") {
  Parser parser = Parser("b(a^)*");
  LogicalVA va = parser.get_logical_va();

  va.remove_epsilon();
  va.remove_useless_anchors();
  va.trim();

  REQUIRE(va.is_accepting_state_reachable());
}
}  // namespace REmatch::testing
