#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "parsing/parser.hpp"

namespace rematch::testing {

int extra_states_in_self_loop = 6;
int extra_transitions_from_initial_state = 4;

bool extended_va_has_only_read_captures_transitions(ExtendedVA* const& extended_va);
bool state_has_self_loop(ExtendedVAState* state);
bool state_has_self_loop_of_length(ExtendedVAState* state, int length);
std::bitset<64> get_close_code(std::bitset<64> code);

TEST_CASE("extended va from '!x{a}' is constructed correctly") {
  Parser parser = Parser("!x{a}");
  LogicalVA logical_va = parser.get_logical_va();

  ExtendedVA extended_va = ExtendedVA(logical_va);

  REQUIRE(extended_va.states.size() == 3 + extra_states_in_self_loop);
  REQUIRE(extended_va_has_only_read_captures_transitions(&extended_va));
}

TEST_CASE("extended va from '!x{!y{a}}' is constructed correctly") {
  Parser parser = Parser("!x{!y{a}}");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  LogicalVACapture* outer_open_capture = logical_va.initial_state()->captures.front();
  LogicalVACapture* inner_open_capture = outer_open_capture->next->captures.front();
  LogicalVAFilter* logical_va_filter = inner_open_capture->next->filters.front();
  LogicalVACapture* inner_close_capture = logical_va_filter->next->captures.front();
  LogicalVACapture* outer_close_capture = inner_close_capture->next->captures.front();

  ExtendedVAState* initial_state = extended_va.initial_state();
  ExtendedVAReadCapture* read_capture_left = initial_state->read_captures.front();
  ExtendedVAReadCapture* read_capture_right = read_capture_left->next->read_captures.front();

  REQUIRE(extended_va.states.size() == 3 + extra_states_in_self_loop);
  REQUIRE(extended_va_has_only_read_captures_transitions(&extended_va));

  REQUIRE(read_capture_left->charclass == logical_va_filter->charclass);
  REQUIRE(read_capture_left->captures_set == (outer_open_capture->code | inner_open_capture->code));

  REQUIRE(read_capture_right->charclass == CharClass(-1));
  REQUIRE(read_capture_right->captures_set == (inner_close_capture->code | outer_close_capture->code));
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
  ExtendedVAReadCapture* eva_upper_open_x = extended_va.initial_state()->read_captures.front();
  ExtendedVAReadCapture* eva_upper_open_y = eva_upper_open_x->next->read_captures.front();
  ExtendedVAReadCapture* eva_upper_close_xy = eva_upper_open_y->next->read_captures.front();

  ExtendedVAReadCapture* eva_lower_open_xy = extended_va.initial_state()->read_captures[1];
  ExtendedVAReadCapture* eva_lower_close_y = eva_lower_open_xy->next->read_captures.front();
  ExtendedVAReadCapture* eva_lower_close_x = eva_lower_close_y->next->read_captures.front();

  REQUIRE(extended_va.states.size() == 6 + extra_states_in_self_loop);
  REQUIRE(extended_va_has_only_read_captures_transitions(&extended_va));

  REQUIRE(eva_upper_open_x->captures_set == lva_open_x->code);
  REQUIRE(eva_upper_open_y->captures_set == lva_open_y->code);
  REQUIRE(eva_upper_close_xy->captures_set == get_close_code(lva_open_x->code | lva_open_y->code));
  REQUIRE(eva_lower_open_xy->captures_set == (lva_open_x->code | lva_open_y->code));
  REQUIRE(eva_lower_close_x->captures_set == get_close_code(lva_open_x->code));
  REQUIRE(eva_lower_close_y->captures_set == get_close_code(lva_open_y->code));

  REQUIRE(eva_upper_open_x->charclass == lva_filter_a->charclass);
  REQUIRE(eva_upper_open_y->charclass == lva_filter_a->charclass);
  REQUIRE(eva_upper_close_xy->charclass == CharClass(-1));
  REQUIRE(eva_lower_open_xy->charclass == lva_filter_a->charclass);
  REQUIRE(eva_lower_close_y->charclass == lva_filter_a->charclass);
  REQUIRE(eva_lower_close_x->charclass == CharClass(-1));
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

  ExtendedVAReadCapture* eva_lower_open_x = extended_va.initial_state()->read_captures.front();
  ExtendedVAReadCapture* eva_upper_open_x = extended_va.initial_state()->read_captures[1];
  ExtendedVAReadCapture* eva_read_a = eva_upper_open_x->next->read_captures[1];
  ExtendedVAReadCapture* eva_close_x = eva_read_a->next->read_captures.front();
  ExtendedVAState* eva_loop_state = eva_upper_open_x->next;

  REQUIRE(extended_va.states.size() == 4 + extra_states_in_self_loop);
  REQUIRE(extended_va_has_only_read_captures_transitions(&extended_va));

  REQUIRE(eva_upper_open_x->captures_set == lva_open_x->code);
  REQUIRE(eva_lower_open_x->captures_set == lva_open_x->code);
  REQUIRE(eva_read_a->captures_set == 0);
  REQUIRE(eva_close_x->captures_set == get_close_code(lva_open_x->code));

  REQUIRE(eva_upper_open_x->charclass == a_charclass);
  REQUIRE(eva_lower_open_x->charclass == a_charclass);
  REQUIRE(eva_read_a->charclass == a_charclass);
  REQUIRE(eva_close_x->charclass == a_charclass);
  REQUIRE(eva_loop_state->read_captures[0]->charclass == a_charclass);
}

TEST_CASE("initial state has a self loop") {
  Parser parser = Parser("a");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  ExtendedVAState* initial_state = extended_va.initial_state();

  REQUIRE(initial_state->read_captures.size() == 5);
  REQUIRE(state_has_self_loop(initial_state));
}

TEST_CASE("duplicate extended va is correct for 'aaa'") {
  Parser parser = Parser("aaa");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  extended_va.duplicate();

  REQUIRE(extended_va.states.size() == 2 * (4 + extra_states_in_self_loop) - 1);

  ExtendedVAState* initial_state = extended_va.initial_state();
  ExtendedVAReadCapture* first_read_capture = initial_state->read_captures.front();
  ExtendedVAReadCapture* second_read_capture = first_read_capture->next->read_captures.front();
  ExtendedVAReadCapture* third_read_capture = second_read_capture->next->read_captures.front();

  ExtendedVAState* initial_state_duplicate = initial_state->read_captures[1]->next;
  ExtendedVAReadCapture* first_read_capture_duplicate = initial_state_duplicate->read_captures.front();
  ExtendedVAReadCapture* second_read_capture_duplicate = first_read_capture_duplicate->next->read_captures.front();
  ExtendedVAReadCapture* third_read_capture_duplicate = second_read_capture_duplicate->next->read_captures.front();

  REQUIRE(initial_state->read_captures.size() == 1 + extra_transitions_from_initial_state);
  REQUIRE(initial_state_duplicate->read_captures.size() == 1 + extra_transitions_from_initial_state);

  REQUIRE(third_read_capture->next == third_read_capture_duplicate->next);
}

TEST_CASE("duplicate extended va is correct for 'a+'") {
  Parser parser = Parser("a+");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  extended_va.duplicate();
  CharClass charclass_a = logical_va.initial_state()->filters.front()->charclass;

  ExtendedVAState* initial_state = extended_va.initial_state();

  std::map<int, int> number_of_transitions_count;
  std::map<int, int> number_of_backward_transitions_count;

  for (auto& state : extended_va.states) {
    int number_of_transitions = state->read_captures.size();
    int number_of_backward_transitions = state->backward_read_captures.size();
    number_of_transitions_count[number_of_transitions]++;
    number_of_backward_transitions_count[number_of_backward_transitions]++;
  }

  REQUIRE(extended_va.states.size() == 5 + 2 * extra_states_in_self_loop);
  REQUIRE(initial_state->read_captures.size() == 2 + extra_transitions_from_initial_state);

  // the initial states have 2 + 4 outgoing transitions
  REQUIRE(number_of_transitions_count[2 + extra_transitions_from_initial_state] == 2);
  // the accepting state has no outgoing transitions
  REQUIRE(number_of_transitions_count[0] == 1);
  REQUIRE(number_of_transitions_count[2] == 2);
  
  // the initial states and the accepting state have 4 incoming transitions
  REQUIRE(number_of_backward_transitions_count[extra_transitions_from_initial_state] == 3);
  REQUIRE(number_of_backward_transitions_count[2] == 2);
}

bool extended_va_has_only_read_captures_transitions(ExtendedVA* const& extended_va) {
  for (auto& state : extended_va->states) {
    if (!(state->filters.empty() && state->backward_filters.empty() &&
          state->captures.empty() && state->backward_captures.empty()))
      return false;
  }

  return true;
}

std::bitset<64> get_close_code(std::bitset<64> code) {
  return code << 1;
}

bool state_has_self_loop(ExtendedVAState* state) {
  return state_has_self_loop_of_length(state, 1) &&
         state_has_self_loop_of_length(state, 2) &&
         state_has_self_loop_of_length(state, 3) &&
         state_has_self_loop_of_length(state, 4);
}

bool state_has_self_loop_of_length(ExtendedVAState* starting_state, int length) {
  std::map<ExtendedVAState*, bool> visited;

  std::stack<ExtendedVAState*> states_stack;
  std::stack<int> length_stack;

  states_stack.push(starting_state);
  length_stack.push(1);

  while (!states_stack.empty()) {
    ExtendedVAState* current_state = states_stack.top(); states_stack.pop();
    visited[current_state] = true;
    int current_length = length_stack.top(); length_stack.pop();

    for (auto& read_capture : current_state->read_captures) {
      if (current_length > length)
        break;

      ExtendedVAState* next_state = read_capture->next;

      if (next_state == starting_state && current_length == length)
        return true;

      if (!visited[next_state]) {
        states_stack.push(next_state);
        length_stack.push(current_length + 1);
      }
    }
  }

  return false;
}

}  // namespace rematch::testing
