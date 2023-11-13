#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/algorithm_class.hpp"
#include "evaluation/extended_va/dfa/extended_det_va.hpp"
#include "library_interface/rematch.hpp"

namespace rematch::testing {

TEST_CASE("memory usage is updated when tracking a state") {
  size_t memory_usage_before = MemoryTracker::get_instance().get_memory_usage();

  auto state = new ExtendedDetVAState();
  size_t memory_usage_after = MemoryTracker::get_instance().get_memory_usage();

  REQUIRE(memory_usage_after - memory_usage_before ==
          sizeof(ExtendedDetVAState));
  delete state;
}

TEST_CASE("memory usage is updated when untracking a state") {
  auto state = new ExtendedDetVAState();
  size_t memory_usage_before = MemoryTracker::get_instance().get_memory_usage();

  delete state;
  size_t memory_usage_after = MemoryTracker::get_instance().get_memory_usage();

  REQUIRE(memory_usage_before - memory_usage_after ==
          sizeof(ExtendedDetVAState));
}

TEST_CASE("memory usage is reset correctly") {
  auto& memory_tracker = MemoryTracker::get_instance();

  memory_tracker.track<int>();
  CHECK(memory_tracker.get_memory_usage() != 0);

  memory_tracker.reset_memory_usage();
  REQUIRE(memory_tracker.get_memory_usage() == 0);
}

TEST_CASE("memory settings can be updated from the regex interface") {
  REMatch::Regex regex = REMatch::compile("!x{a}");
  auto& memory_tracker = MemoryTracker::get_instance();
  REQUIRE(regex.get_memory_limit() == memory_tracker.get_memory_limit());

  regex.set_memory_limit(sizeof(char));
  REQUIRE(regex.get_memory_limit() == memory_tracker.get_memory_limit());

  memory_tracker.reset_memory_limit();
}

TEST_CASE(
    "memory tracker raises an exception when the memory limit is exceeded") {
  auto& memory_tracker = MemoryTracker::get_instance();
  memory_tracker.reset_memory_usage();
  memory_tracker.set_memory_limit(sizeof(uint8_t));

  REQUIRE_THROWS_AS(memory_tracker.track<uint64_t>(),
                    MemoryLimitExceededException);
  memory_tracker.reset_memory_limit();
}

}  // namespace rematch::testing
