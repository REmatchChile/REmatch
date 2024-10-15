#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "evaluation/start_end_chars.hpp"
#include "parsing/parser.hpp"
#include "../tests_utils/tests_utils.hpp"

namespace REmatch::testing {

CharClass asterisk_class = CharClass({'\x80', '\x7F'});

TEST_CASE("extended va from '!x{a}' is constructed correctly") {
  Parser parser = Parser("!x{a}");
  LogicalVA logical_va = parser.get_logical_va();

  ExtendedVA extended_va = ExtendedVA(logical_va);

  REQUIRE(extended_va.states.size() == 3);
  REQUIRE(extended_va_has_only_read_captures_transitions(extended_va));
}

TEST_CASE("extended va from '!x{!y{a}}' is constructed correctly") {
  Parser parser = Parser("!x{!y{a}}");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  LogicalVACapture* outer_open_capture =
      logical_va.initial_state()->captures.front();
  LogicalVACapture* inner_open_capture =
      outer_open_capture->next->captures.front();
  LogicalVAFilter* logical_va_filter =
      inner_open_capture->next->filters.front();
  LogicalVACapture* inner_close_capture =
      logical_va_filter->next->captures.front();
  LogicalVACapture* outer_close_capture =
      inner_close_capture->next->captures.front();

  ExtendedVAState* initial_state = extended_va.initial_state();
  ExtendedVAReadCapture* read_capture_left =
      initial_state->read_captures.front();
  ExtendedVAReadCapture* read_capture_right =
      read_capture_left->next->read_captures.front();

  REQUIRE(extended_va.states.size() == 3);
  REQUIRE(extended_va_has_only_read_captures_transitions(extended_va));

  REQUIRE(read_capture_left->charclass == logical_va_filter->charclass);
  REQUIRE(read_capture_left->captures_set ==
          (outer_open_capture->code | inner_open_capture->code));

  REQUIRE(read_capture_right->charclass == asterisk_class);
  REQUIRE(read_capture_right->captures_set ==
          (inner_close_capture->code | outer_close_capture->code));
}

TEST_CASE("extended va from '!x{(a!y{a}|!y{a}a)}' is constructed correctly") {
  Parser parser = Parser("!x{(a!y{a}|!y{a}a)}");
  LogicalVA logical_va = parser.get_logical_va();
  logical_va.remove_epsilon();
  logical_va.trim();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  // logical va transitions
  LogicalVACapture* lva_open_x = logical_va.initial_state()->captures.front();
  LogicalVACapture* lva_open_y = lva_open_x->next->captures.front();
  LogicalVAFilter* lva_filter_a = lva_open_x->next->filters.front();

  // extended va transitions
  ExtendedVAReadCapture* eva_upper_open_x =
      extended_va.initial_state()->read_captures.front();
  ExtendedVAReadCapture* eva_upper_open_y =
      eva_upper_open_x->next->read_captures.front();
  ExtendedVAReadCapture* eva_upper_close_xy =
      eva_upper_open_y->next->read_captures.front();

  ExtendedVAReadCapture* eva_lower_open_xy =
      extended_va.initial_state()->read_captures[1];
  ExtendedVAReadCapture* eva_lower_close_y =
      eva_lower_open_xy->next->read_captures.front();
  ExtendedVAReadCapture* eva_lower_close_x =
      eva_lower_close_y->next->read_captures.front();

  REQUIRE(extended_va.states.size() == 6);
  REQUIRE(extended_va_has_only_read_captures_transitions(extended_va));

  REQUIRE(eva_upper_open_x->captures_set == lva_open_x->code);
  REQUIRE(eva_upper_open_y->captures_set == lva_open_y->code);
  REQUIRE(eva_upper_close_xy->captures_set ==
          get_close_code(lva_open_x->code | lva_open_y->code));
  REQUIRE(eva_lower_open_xy->captures_set ==
          (lva_open_x->code | lva_open_y->code));
  REQUIRE(eva_lower_close_x->captures_set == get_close_code(lva_open_x->code));
  REQUIRE(eva_lower_close_y->captures_set == get_close_code(lva_open_y->code));

  REQUIRE(eva_upper_open_x->charclass == lva_filter_a->charclass);
  REQUIRE(eva_upper_open_y->charclass == lva_filter_a->charclass);
  REQUIRE(eva_upper_close_xy->charclass == asterisk_class);
  REQUIRE(eva_lower_open_xy->charclass == lva_filter_a->charclass);
  REQUIRE(eva_lower_close_y->charclass == lva_filter_a->charclass);
  REQUIRE(eva_lower_close_x->charclass == asterisk_class);
}

TEST_CASE("extended va from '!x{a+}a' is constructed correctly") {
  Parser parser = Parser("!x{a+}a");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  logical_va.remove_epsilon();
  logical_va.trim();

  LogicalVACapture* lva_open_x = logical_va.initial_state()->captures.front();
  LogicalVAFilter* lva_filter_a = lva_open_x->next->filters.front();
  CharClass a_charclass = lva_filter_a->charclass;

  ExtendedVAState* eva_initial_state = extended_va.initial_state();
  REQUIRE(extended_va.states.size() == 3);
  REQUIRE(extended_va_has_only_read_captures_transitions(extended_va));

  REQUIRE(eva_initial_state->read_captures.size() == 1);
  ExtendedVAReadCapture* read_open_x = eva_initial_state->read_captures.front();
  REQUIRE(read_open_x->captures_set == lva_open_x->code);
  REQUIRE(read_open_x->charclass == a_charclass);

  ExtendedVAState* loop_state = eva_initial_state->read_captures.front()->next;
  REQUIRE(loop_state->read_captures.size() == 2);

  ExtendedVAReadCapture* loop_transition = loop_state->read_captures.back();
  REQUIRE(loop_transition->captures_set == 0);
  REQUIRE(loop_transition->charclass == a_charclass);

  ExtendedVAReadCapture* read_close_x = loop_state->read_captures.front();
  REQUIRE(read_close_x->captures_set == get_close_code(lva_open_x->code));
  REQUIRE(read_close_x->charclass == a_charclass);

  ExtendedVAState* final_state = loop_state->read_captures.front()->next;
  REQUIRE(final_state->is_accepting());
}

TEST_CASE("extended va with start anchor is constructed correctly") {
  Parser parser = Parser("^a");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  REQUIRE(extended_va.states.size() == 3);
  ExtendedVAState* initial_state = extended_va.initial_state();

  REQUIRE(initial_state->read_captures.size() == 1);
  REQUIRE(initial_state->read_captures[0]->charclass == CharClass(START_CHAR));
  REQUIRE(initial_state->read_captures[0]->captures_set == 0);
}

TEST_CASE("extended va with end anchor is constructed correctly") {
  Parser parser = Parser("a$");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  REQUIRE(extended_va.states.size() == 3);
  ExtendedVAState* accepting_state = extended_va.accepting_state();

  REQUIRE(accepting_state->backward_read_captures.size() == 1);
  ExtendedVAReadCapture* end_anchor_transition =
      accepting_state->backward_read_captures[0];
  REQUIRE(end_anchor_transition->charclass == CharClass(END_CHAR));
  REQUIRE(end_anchor_transition->captures_set == 0);
}

TEST_CASE("extended va with multi spanners is constructed correctly") {
  Parser parser = Parser("!x{a}+", true);
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  std::bitset<64> open_code =
      logical_va.initial_state()->captures.front()->code;
  std::bitset<64> close_code = get_close_code(open_code);

  REQUIRE(extended_va.states.size() == 3);
  ExtendedVAState* initial_state = extended_va.initial_state();
  REQUIRE(initial_state->read_captures.size() == 1);
  REQUIRE(initial_state->read_captures[0]->captures_set == open_code);

  ExtendedVAState* middle_state = initial_state->read_captures[0]->next;
  REQUIRE(middle_state->read_captures.size() == 2);

  ExtendedVAReadCapture* loop_transition = middle_state->read_captures[1];
  REQUIRE(loop_transition->next == middle_state);
  REQUIRE(loop_transition->captures_set == (open_code | close_code));

  ExtendedVAReadCapture* close_transition = middle_state->read_captures[0];
  REQUIRE(close_transition->captures_set == close_code);

  ExtendedVAState* final_state = close_transition->next;
  REQUIRE(final_state->is_accepting());
}

TEST_CASE("initial state has a self loop") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  extended_va.add_loop_to_initial_state();

  ExtendedVAState* initial_state = extended_va.initial_state();

  REQUIRE(initial_state->read_captures.size() == 2);
  REQUIRE(state_has_self_loop(initial_state));
}

TEST_CASE("duplicate extended va is correct for 'aaa'") {
  // this test checks the correctness of the duplication
  // without adding the initial loop
  Parser parser = Parser("aaa");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  extended_va.duplicate();

  REQUIRE(extended_va.size() == 4);

  ExtendedVAState* initial_state = extended_va.initial_state();
  ExtendedVAState* second_state = initial_state->read_captures.front()->next;
  ExtendedVAState* third_state = second_state->read_captures.front()->next;
  ExtendedVAState* accepting_state = extended_va.accepting_state();

  REQUIRE(initial_state->read_captures.size() == 1);
  REQUIRE(second_state->read_captures.size() == 1);
  REQUIRE(third_state->read_captures.size() == 1);
  REQUIRE(accepting_state == third_state->read_captures.front()->next);
}

TEST_CASE("duplicate extended va is correct for 'a+'") {
  Parser parser = Parser("a+");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  extended_va.duplicate();

  ExtendedVAState* initial_state = extended_va.initial_state();

  REQUIRE(initial_state->read_captures.size() == 2);
  ExtendedVAState* lower_state = initial_state->read_captures.front()->next;
  ExtendedVAState* accepting_state = initial_state->read_captures.back()->next;

  REQUIRE(lower_state->read_captures.size() == 2);
  ExtendedVAState* upper_state = lower_state->read_captures[0]->next;

  REQUIRE(upper_state->read_captures.size() == 2);
  REQUIRE(upper_state->read_captures[0]->next == lower_state);
  REQUIRE(upper_state->read_captures[1]->next == accepting_state);

  // -2 because initial and accepting states are removed
  REQUIRE(extended_va.states.size() == 2 * 3 - 2);
  REQUIRE(accepting_state->is_accepting());
}

TEST_CASE("duplicate extended va is correct for '!x{a*b}'") {
  Parser parser = Parser("!x{a*b}");
  LogicalVA logical_va = parser.get_logical_va();
  logical_va.remove_epsilon();
  logical_va.trim();

  ExtendedVA extended_va = ExtendedVA(logical_va);

  extended_va.duplicate();

  REQUIRE(extended_va.size() == 4 * 2 - 2);

  ExtendedVAState* initial_state = extended_va.initial_state();
  REQUIRE(initial_state->read_captures.size() == 2);
  ExtendedVAState* second_state2 = initial_state->read_captures[0]->next;
  ExtendedVAState* loop_state2 = initial_state->read_captures[1]->next;

  REQUIRE(loop_state2->read_captures.size() == 2);
  ExtendedVAState* second_state1 = loop_state2->read_captures[0]->next;
  ExtendedVAState* loop_state1 = loop_state2->read_captures[1]->next;

  REQUIRE(loop_state1->read_captures.size() == 2);
  REQUIRE(loop_state1->read_captures[0]->next == second_state2);
  REQUIRE(loop_state1->read_captures[1]->next == loop_state2);

  REQUIRE(second_state1->read_captures.size() == 1);
  ExtendedVAState* final_state = second_state1->read_captures[0]->next;

  REQUIRE(second_state2->read_captures.size() == 1);
  REQUIRE(second_state2->read_captures[0]->next == final_state);
  REQUIRE(final_state->is_accepting());
}

TEST_CASE("the capture reaching the final state reads any character") {
  Parser parser = Parser("!x{a}");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  ExtendedVAState* initial_state = extended_va.initial_state();
  ExtendedVAReadCapture* open_x_capture = initial_state->read_captures[0];
  ExtendedVAReadCapture* close_x_capture =
      open_x_capture->next->read_captures[0];

  REQUIRE(charclass_contains_every_character(close_x_capture->charclass));
}

TEST_CASE("relabel states is correct") {
  Parser parser = Parser("a+");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  extended_va.relabel_states();

  size_t max_id = get_max_id(extended_va);
  REQUIRE(get_min_id(extended_va) == 0);
  REQUIRE(extended_va.size() == max_id + 1);
}

}  // namespace REmatch::testing
