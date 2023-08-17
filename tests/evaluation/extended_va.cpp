#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "parsing/parser.hpp"

namespace rematch::testing {

bool extended_va_has_only_read_captures_transitions(ExtendedVA* const& extended_va);
bool state_has_self_loop(ExtendedVAState* state);
std::bitset<64> get_close_code(std::bitset<64> code);
int get_max_id(ExtendedVA* const& extended_va);
int get_min_id(ExtendedVA* const& extended_va);

TEST_CASE("extended va from '!x{a}' is constructed correctly") {
  Parser parser = Parser("!x{a}");
  LogicalVA logical_va = parser.get_logical_va();

  ExtendedVA extended_va = ExtendedVA(logical_va);

  REQUIRE(extended_va.states.size() == 3);
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

  REQUIRE(extended_va.states.size() == 3);
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

  REQUIRE(extended_va.states.size() == 6);
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

  REQUIRE(extended_va.states.size() == 4);
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

  extended_va.add_loop_to_initial_state();

  ExtendedVAState* initial_state = extended_va.initial_state();

  REQUIRE(initial_state->read_captures.size() == 2);
  REQUIRE(state_has_self_loop(initial_state));
}

TEST_CASE("duplicate extended va is correct for 'aaa'") {
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
  ExtendedVAState* lower_state = initial_state->read_captures.back()->next;
  ExtendedVAState* accepting_state = initial_state->read_captures.front()->next;

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
  
  std::cout << extended_va << std::endl;

  extended_va.duplicate();

  std::cout << extended_va << std::endl;

  REQUIRE(extended_va.size() == 5 * 2 - 2);

  ExtendedVAState* initial_state = extended_va.initial_state();
  REQUIRE(initial_state->read_captures.size() == 3);

  ExtendedVAReadCapture* open_x_read_a_lower = initial_state->read_captures[0];
  ExtendedVAReadCapture* open_x_read_a_upper = initial_state->read_captures[1];
  ExtendedVAReadCapture* open_x_read_b = initial_state->read_captures[2];

  REQUIRE(open_x_read_a_lower->next->read_captures.size() == 1);
  REQUIRE(open_x_read_b->next->read_captures.size() == 1);

  REQUIRE(open_x_read_a_upper->next->read_captures.size() == 2);
  ExtendedVAReadCapture* loop_read_capture = open_x_read_a_upper->next->read_captures[0];
  ExtendedVAReadCapture* read_a = open_x_read_a_upper->next->read_captures[1];

  REQUIRE(read_a->next->read_captures.size() == 1);
  ExtendedVAReadCapture* read_b = read_a->next->read_captures[0];
  ExtendedVAReadCapture* close_x_read_eof = read_b->next->read_captures[0];

  REQUIRE(loop_read_capture->next->read_captures.size() == 2);
  ExtendedVAReadCapture* read_a_duplicate = loop_read_capture->next->read_captures[1];
  ExtendedVAReadCapture* read_b_duplicate = read_a_duplicate->next->read_captures[0];
  ExtendedVAReadCapture* close_x_read_eof_dup = read_b_duplicate->next->read_captures[0];


  REQUIRE(open_x_read_a_lower->next->read_captures.size() == 1);
  REQUIRE(close_x_read_eof->next == extended_va.accepting_state());
  REQUIRE(close_x_read_eof_dup->next == extended_va.accepting_state());
}

TEST_CASE("relabel states is correct") {
  Parser parser = Parser("a+");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  extended_va.relabel_states();

  int max_id = get_max_id(&extended_va);
  REQUIRE(get_min_id(&extended_va) == 0);
  REQUIRE(extended_va.size() == max_id + 1);
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
  for (auto& read_capture : state->read_captures) {
    if (read_capture->next == state)
      return true;
  }

  return false;
}

int get_max_id(ExtendedVA* const& extended_va) {
  unsigned int max_id = 0;

  for (auto &state : extended_va->states) {
    max_id = std::max(max_id, state->id);
  }

  return max_id;
}

int get_min_id(ExtendedVA* const& extended_va) {
  unsigned int min_id = extended_va->states.front()->id;

  for (auto &state : extended_va->states) {
    min_id = std::min(min_id, state->id);
  }

  return min_id;
}

}  // namespace rematch::testing
