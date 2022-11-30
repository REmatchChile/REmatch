//
// Created by Nicolás Van Sint Jan on 09-11-22.
//
#include <catch2/catch_test_macros.hpp>

#include "regex/regex.hpp"
#include "evaluation/stats.hpp"
#include "evaluation/segment_evaluator.hpp"

namespace rematch::testing {

TEST_CASE("Segments correct", "[segments]") {
  RegExOptions opts;
  opts.set_searching(true);

  auto rgx = RegEx("!x{a+}", opts);
  EvalStats stats;

  auto eval = SegmentEvaluator{rgx, "aabbabaaa", Anchor::kUnanchored, stats};

  REQUIRE(eval.i_src_ == 0);
  REQUIRE(eval.i_min_ == 0);
  REQUIRE(eval.i_max_ == 0);

  eval.searching_phase();

  REQUIRE(eval.i_src_ == 2);
  REQUIRE(eval.i_min_ == 0);
  REQUIRE(eval.i_max_ == 2);

  eval.searching_phase();

  REQUIRE(eval.i_src_ == 5);
  REQUIRE(eval.i_min_ == 3);
  REQUIRE(eval.i_max_ == 5);

  eval.searching_phase();

  REQUIRE(eval.i_src_ == 9);
  REQUIRE(eval.i_min_ == 5);
  REQUIRE(eval.i_max_ == 9);
}

}