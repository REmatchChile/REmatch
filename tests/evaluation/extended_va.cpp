#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/extended_va/nfa/extended_va.hpp"
#include "parsing/parser.hpp"

namespace rematch::testing {

bool extended_va_has_only_read_captures_transitions(ExtendedVA* const& extended_va);
bool state_has_self_loop(ExtendedVAState* state);
std::bitset<64> get_close_code(std::bitset<64> code);

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


TEST_CASE("initial state has a self loop") {
  Parser parser = Parser("!x{a}");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);

  ExtendedVAState* initial_state = extended_va.initial_state();

  REQUIRE(state_has_self_loop(initial_state));

  ExtendedVAReadCapture* loop_read_capture = initial_state->read_captures.back();

  std::bitset<64> empty_set;
  CharClass asterisk_class = CharClass((char) 0, (char) 255);
  REQUIRE(loop_read_capture->captures_set == empty_set);
  REQUIRE(loop_read_capture->charclass == asterisk_class);
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
  for (ExtendedVAReadCapture* read_capture : state->read_captures) {
    if (read_capture->next == state)
      return true;
  }
  return false;
}

TEST_CASE("dot") {
  Parser parser = Parser(".");
  LogicalVA logical_va = parser.get_logical_va();
  std::cout << logical_va << std::endl;
}


}  // namespace rematch::testing
