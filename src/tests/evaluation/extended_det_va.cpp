#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "../tests_utils/tests_utils.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "parsing/parser.hpp"

namespace REmatch::testing {

TEST_CASE("initial state is created correctly") {
  ExtendedVA extended_va = get_extended_va_from_query("a");
  ExtendedDetVA extended_det_va(extended_va);
  ExtendedDetVAState* initial_state = extended_det_va.get_initial_state();
  std::vector<ExtendedVAState*> states_subset =
      initial_state->get_states_subset();

  REQUIRE(extended_det_va.states.size() == 1);
  REQUIRE(initial_state->is_initial());
  REQUIRE(initial_state->get_states_subset().size() == 1);
}

TEST_CASE("next state is computed correctly when there is a valid transition") {
  Parser parser = Parser("!x{a}");
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  extended_va.clean_for_determinization();
  ExtendedDetVA extended_det_va = ExtendedDetVA(extended_va);

  std::bitset<64> eva_open_x_code =
      extended_va.initial_state()->read_captures[0]->captures_set;
  std::bitset<64> eva_empty_code =
      extended_va.initial_state()->read_captures[1]->captures_set;

  ExtendedDetVAState* initial_state = extended_det_va.get_initial_state();

  std::vector<CaptureSubsetPair> capture_subset_list;
  capture_subset_list = extended_det_va.get_next_states(initial_state, 'a');

  // it has 3 states: initial state, the duplicate of the initial state and
  // the state reached when reading 'a'
  REQUIRE(extended_det_va.states.size() == 3);
  REQUIRE(capture_subset_list.size() == 2);

  std::bitset<64> det_va_empty_code = capture_subset_list[0].capture;
  ExtendedDetVAState* initial_state_duplicate = capture_subset_list[0].subset;

  REQUIRE(initial_state_duplicate->get_subset_size() == 1);
  REQUIRE(det_va_empty_code == eva_empty_code);

  std::bitset<64> det_va_open_x_code = capture_subset_list[1].capture;
  ExtendedDetVAState* second_state = capture_subset_list[1].subset;

  REQUIRE(second_state->get_subset_size() == 1);
  REQUIRE(det_va_open_x_code == eva_open_x_code);
}

TEST_CASE(
    "next state is computed correctly when there are no valid transitions other \
           than the initial loop") {
  ExtendedVA extended_va = get_extended_va_from_query("!x{a}");
  ExtendedDetVA extended_det_va = ExtendedDetVA(extended_va);

  ExtendedDetVAState* initial_state = extended_det_va.get_initial_state();

  std::vector<CaptureSubsetPair> capture_subset_list;
  capture_subset_list = extended_det_va.get_next_states(initial_state, 'b');

  REQUIRE(extended_det_va.states.size() == 2);
  REQUIRE(capture_subset_list.size() == 1);

  std::bitset<64> empty_capture;
  REQUIRE(capture_subset_list[0].capture == empty_capture);
  REQUIRE(capture_subset_list[0].subset->get_subset_size() == 1);
}

TEST_CASE(
    "next state is computed correctly when there are two transitions with the same \
           capture and reading") {

  // the initial state of the extended VA has two 'a/[x' transitions
  ExtendedVA extended_va = get_extended_va_from_query("!x{a+|a}");
  ExtendedDetVA extended_det_va = ExtendedDetVA(extended_va);

  ExtendedDetVAState* initial_state = extended_det_va.get_initial_state();

  std::vector<CaptureSubsetPair> capture_subset_list;
  capture_subset_list = extended_det_va.get_next_states(initial_state, 'a');

  REQUIRE(capture_subset_list.size() == 2);

  ExtendedDetVAState* second_state = capture_subset_list[1].subset;

  REQUIRE(second_state->get_subset_size() == 2);
}

TEST_CASE(
    "next state is computed correctly when the current subset has more than "
    "one state") {
  ExtendedVA extended_va = get_extended_va_from_query("!x{aa|aa}");
  ExtendedDetVA extended_det_va = ExtendedDetVA(extended_va);

  ExtendedDetVAState* initial_state = extended_det_va.get_initial_state();

  std::vector<CaptureSubsetPair> capture_subset_list;
  capture_subset_list = extended_det_va.get_next_states(initial_state, 'a');

  ExtendedDetVAState* second_state = capture_subset_list[1].subset;
  capture_subset_list = extended_det_va.get_next_states(second_state, 'a');

  REQUIRE(capture_subset_list.size() == 1);

  std::bitset<64> empty_capture;

  REQUIRE(capture_subset_list[0].capture == empty_capture);
  REQUIRE(capture_subset_list[0].subset->get_subset_size() == 2);
}

TEST_CASE(
    "next state is returned immediately when it has already been computed "
    "before") {
  ExtendedVA extended_va = get_extended_va_from_query("!y{a}");
  ExtendedDetVA extended_det_va = ExtendedDetVA(extended_va);

  ExtendedDetVAState* initial_state = extended_det_va.get_initial_state();

  std::vector<CaptureSubsetPair> capture_subset_list1;
  capture_subset_list1 = extended_det_va.get_next_states(initial_state, 'a');
  ExtendedDetVAState* next_state1 = capture_subset_list1[1].subset;

  std::vector<CaptureSubsetPair> capture_subset_list2;
  capture_subset_list2 = extended_det_va.get_next_states(initial_state, 'a');
  ExtendedDetVAState* next_state2 = capture_subset_list1[1].subset;

  REQUIRE(next_state1->id == next_state2->id);
}

TEST_CASE(
    "the deterministic state is final if a state in the subset is final") {
  ExtendedVA extended_va = get_extended_va_from_query("a");
  ExtendedDetVA extended_det_va = ExtendedDetVA(extended_va);

  ExtendedDetVAState* initial_state = extended_det_va.get_initial_state();

  std::vector<CaptureSubsetPair> capture_subset_list;
  capture_subset_list = extended_det_va.get_next_states(initial_state, 'a');

  ExtendedDetVAState* second_state = capture_subset_list[0].subset;

  REQUIRE(second_state->is_accepting());
}

TEST_CASE("non ascii characters are handled correctly") {
  ExtendedVA extended_va = get_extended_va_from_query("a");
  ExtendedDetVA extended_det_va = ExtendedDetVA(extended_va);

  ExtendedDetVAState* initial_state = extended_det_va.get_initial_state();

  std::vector<CaptureSubsetPair> capture_subset_list;
  capture_subset_list =
      extended_det_va.get_next_states(initial_state, END_CHAR);

  REQUIRE(capture_subset_list.size() == 1);
}

}  // namespace REmatch::testing
