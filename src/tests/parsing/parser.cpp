#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "parsing/logical_variable_set_automaton/logical_va_filter.hpp"
#include "parsing/parser.hpp"
#include "parsing/visitors/char_class_visitor.hpp"

namespace REmatch::testing {

bool charclass_contains_range(LogicalVAFilter filter, char lower, char upper) {
  for (char current_char = lower; current_char < upper + 1; current_char++) {
    if (filter.charclass.contains(current_char) == false)
      return false;
  }
  return true;
}

bool state_has_transition_for_digit(LogicalVAState& state) {
  for (auto& filter : state.filters) {
    if (charclass_contains_range(*filter, '0', '9')) {
      return true;
    }
  }
  return false;
}

bool charclass_contains_character(LogicalVAFilter filter, char character) {
  return charclass_contains_range(filter, character, character);
}

TEST_CASE("the regex 'a' is parsed correctly into a LogicalVA.") {
  Parser parser = Parser("a");
  LogicalVA va = parser.get_logical_va();
  INFO(va.initial_state()->filters.back()->charclass);
  REQUIRE(va.states.size() == 2);
  REQUIRE(va.initial_state()->filters.size() == 1);
  REQUIRE(va.initial_state()->captures.empty());
  REQUIRE(va.initial_state()->filters.back()->charclass.contains('a'));
  REQUIRE(va.initial_state()->filters.back()->next == va.accepting_state());
  REQUIRE(va.accepting_state()->backward_filters_.size() == 1);
  REQUIRE(va.accepting_state()->accepting());
  REQUIRE(va.accepting_state()->backward_filters_.back()->next ==
          va.accepting_state());
}

TEST_CASE("the regex '!x{a}' is parsed correctly into a LogicalVA.") {
  Parser parser = Parser("!x{a}");
  LogicalVA va = parser.get_logical_va();
  // initial - !x{ -> 2 - a -> 3 - !x} -> final
  REQUIRE(va.states.size() == 4);
  REQUIRE(va.initial_state()->captures.size() == 1);
  REQUIRE(va.initial_state()->captures.front()->next->filters.size() == 1);
  LogicalVAState* second_state = va.initial_state()->captures.front()->next;
  REQUIRE(second_state->filters.back()->charclass.contains('a'));
  LogicalVAState* third_state = second_state->filters.back()->next;
  LogicalVAState* fourth_state = third_state->captures.back()->next;
  REQUIRE(fourth_state->accepting());
  REQUIRE((va.initial_state()->captures.back()->code << 1) != 0);
  REQUIRE((va.initial_state()->captures.back()->code << 1) ==
          third_state->captures.back()->code);
}

TEST_CASE(
    "the regex 'ab' is parsed correctly into a LogicalVA. \
        (after minimization)") {
  Parser parser = Parser("ab");
  LogicalVA va = parser.get_logical_va();
  va.remove_epsilon();
  va.trim();
  INFO(va.initial_state()->filters.back()->charclass);
  REQUIRE(va.states.size() == 3);
  REQUIRE(va.initial_state()->filters.size() == 1);
  REQUIRE(va.initial_state()->captures.empty());
  REQUIRE(va.initial_state()->filters.back()->charclass.contains('a'));
  LogicalVAState* second_state = va.initial_state()->filters.back()->next;
  REQUIRE(second_state->backward_filters_.size() == 1);
  REQUIRE(second_state->filters.size() == 1);
  REQUIRE(second_state->filters.back()->charclass.contains('b'));
  LogicalVAState* final_state = second_state->filters.back()->next;
  REQUIRE(final_state->accepting());
}

TEST_CASE(
    "the regex 'α' is parsed correctly into a LogicalVA. \
          (after minimization)") {
  Parser parser = Parser("α");
  LogicalVA va = parser.get_logical_va();
  va.remove_epsilon();
  va.trim();
  INFO(va.initial_state()->filters.back()->charclass);
  REQUIRE(va.states.size() == 3);
  REQUIRE(va.initial_state()->captures.empty());
  REQUIRE(va.initial_state()->filters.size() == 1);
  REQUIRE(va.initial_state()->filters.back()->charclass.contains((char)0xce));
  LogicalVAState* second_state = va.initial_state()->filters.back()->next;
  REQUIRE(second_state->backward_filters_.size() == 1);
  REQUIRE(second_state->filters.size() == 1);
  REQUIRE(second_state->filters.back()->charclass.contains((char)0xb1));
  REQUIRE(second_state->filters.back());
}

TEST_CASE("the regex '.α' has 13 states  (after minimization)") {
  Parser parser = Parser(".α");
  LogicalVA va = parser.get_logical_va();
  va.remove_epsilon();
  va.trim();
  INFO(va.initial_state()->filters.size());
  INFO(va.initial_state()->filters.back()->charclass);
  REQUIRE(va.states.size() == 13);
  REQUIRE(va.initial_state()->filters.size() == 4);
}

TEST_CASE("the regex '^a' has start anchor") {
  Parser parser = Parser("^a");
  LogicalVA va = parser.get_logical_va();

  INFO(va.states.size());
  INFO(va.initial_state()->anchors.size());
  REQUIRE(va.initial_state()->anchors.size() == 1);
  REQUIRE(va.initial_state()->anchors.front()->is_start() == true);
}

TEST_CASE("the regex 'a$' has end anchor") {
  Parser parser = Parser("a$");
  LogicalVA va = parser.get_logical_va();

  INFO(va.states.size());
  INFO(va.initial_state()->anchors.size());
  REQUIRE(va.initial_state()->anchors.size() == 0);
  REQUIRE(va.accepting_state()->backward_anchors_.size() == 1);
  REQUIRE(va.accepting_state()->backward_anchors_.front()->is_start() == false);
}

TEST_CASE("the anchor in the regex '(^|a)' is parsed correctly") {
  Parser parser = Parser("(^|a)");
  LogicalVA va = parser.get_logical_va();

  INFO(va.states.size());

  LogicalVAState* initial_state = va.initial_state();
  REQUIRE(initial_state->epsilons.size() == 2);

  LogicalVAState* second_state = initial_state->epsilons.front()->next;
  REQUIRE(second_state->anchors.size() == 1);
  REQUIRE(second_state->anchors.front()->is_start());
}

TEST_CASE("regex with char classes is parsed correctly") {
  Parser parser = Parser("[abc][a]");
  LogicalVA logical_va = parser.get_logical_va();
  logical_va.remove_epsilon();
  logical_va.trim();

  REQUIRE(logical_va.states.size() == 3);
  LogicalVAState* initial_state = logical_va.initial_state();
  REQUIRE(initial_state->filters.size() == 1);
  REQUIRE(initial_state->filters.back()->charclass.contains('a'));
  REQUIRE(initial_state->filters.back()->charclass.contains('b'));
  REQUIRE(initial_state->filters.back()->charclass.contains('c'));

  LogicalVAState* second_state = initial_state->filters.back()->next;
  REQUIRE(second_state->filters.size() == 1);
  REQUIRE(second_state->filters.back()->charclass.contains('a'));
  REQUIRE(second_state->filters.back()->charclass.contains('b') == false);
  REQUIRE(second_state->filters.back()->charclass.contains('c') == false);

  LogicalVAState* third_state = second_state->filters.back()->next;
  REQUIRE(third_state->accepting());
}

TEST_CASE("regex with \\d is parsed correctly") {
  Parser parser = Parser("\\d+\\d");
  LogicalVA logical_va = parser.get_logical_va();
  logical_va.remove_epsilon();
  logical_va.trim();

  REQUIRE(logical_va.states.size() == 3);
  LogicalVAState* initial_state = logical_va.initial_state();
  REQUIRE(initial_state->filters.size() == 1);

  LogicalVAState* second_state = initial_state->filters.front()->next;
  REQUIRE(second_state->filters.size() == 2);
  REQUIRE(second_state->filters.back()->next == second_state);
  REQUIRE(charclass_contains_range(*second_state->filters.back(), '0', '9'));
  REQUIRE(charclass_contains_range(*second_state->filters.front(), '0', '9'));

  LogicalVAState* third_state = second_state->filters.front()->next;
  REQUIRE(third_state->accepting());
}

TEST_CASE("regex with \\s is parsed correctly") {
  Parser parser = Parser("\\s*[1-3]");
  LogicalVA logical_va = parser.get_logical_va();
  logical_va.remove_epsilon();
  logical_va.trim();

  REQUIRE(logical_va.states.size() == 3);
  LogicalVAState* initial_state = logical_va.initial_state();
  REQUIRE(initial_state->filters.size() == 2);
  REQUIRE(charclass_contains_range(*initial_state->filters.front(), '1', '3'));
  REQUIRE(charclass_contains_range(*initial_state->filters.back(), 9, 13));
  REQUIRE(charclass_contains_character(*initial_state->filters.back(), 32));

  LogicalVAState* loop_state = initial_state->filters.back()->next;
  REQUIRE(loop_state->filters.size() == 2);
  REQUIRE(loop_state->filters.back()->next == loop_state);
  REQUIRE(charclass_contains_range(*loop_state->filters.back(), 9, 13));
  REQUIRE(charclass_contains_character(*loop_state->filters.back(), 32));
  REQUIRE(charclass_contains_range(*loop_state->filters.front(), '1', '3'));

  LogicalVAState* final_state = loop_state->filters.front()->next;
  REQUIRE(final_state == initial_state->filters.front()->next);
  REQUIRE(final_state->accepting());
}

TEST_CASE("regex with \\w is parsed correctly") {
  Parser parser = Parser("\\w|\\d");
  LogicalVA logical_va = parser.get_logical_va();
  logical_va.remove_epsilon();
  logical_va.trim();

  REQUIRE(logical_va.states.size() == 2);
  LogicalVAState* initial_state = logical_va.initial_state();
  REQUIRE(initial_state->filters.size() == 2);

  LogicalVAFilter* transition_w = initial_state->filters.back();
  REQUIRE(charclass_contains_range(*transition_w, 48, 57));
  REQUIRE(charclass_contains_range(*transition_w, 65, 90));
  REQUIRE(charclass_contains_character(*transition_w, 95));
  REQUIRE(charclass_contains_range(*transition_w, 97, 122));

  LogicalVAFilter* transition_d = initial_state->filters.front();
  REQUIRE(charclass_contains_range(*transition_d, '0', '9'));
  REQUIRE(charclass_contains_range(*transition_d, 65, 90) == false);
  REQUIRE(charclass_contains_character(*transition_d, 95) == false);
  REQUIRE(charclass_contains_range(*transition_d, 97, 122) == false);

  REQUIRE(transition_d->next == transition_w->next);
  REQUIRE(transition_d->next->accepting());
}

TEST_CASE("regex with negated char classes") {
  Parser parser = Parser("\\D");
  LogicalVA logical_va = parser.get_logical_va();
  logical_va.remove_epsilon();
  logical_va.trim();

  LogicalVAState* initial_state = logical_va.initial_state();

  REQUIRE(state_has_transition_for_digit(*initial_state) == false);
}

}  // namespace REmatch::testing
