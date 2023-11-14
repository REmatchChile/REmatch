#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "library_interface/rematch.hpp"
#include "../evaluation/dummy_mapping.hpp"
#include "../evaluation/mapping_helpers.hpp"
#include "exceptions/invalid_flag_type_exception.hpp"
#include "regex"

namespace rematch::testing {

std::string get_mapping_info(DummyMapping mapping);
void run_client_test(MatchIterator& match_iterator, std::vector<DummyMapping> expected_matches);

TEST_CASE("passing the wrong number of arguments when creating a Flags object raises "
          "an exception") {
  REQUIRE_THROWS_AS(Flags(2, Flags::EarlyOutput), InvalidFlagTypeException);
}

TEST_CASE("flag LineByLine works correctly") {
  std::string pattern = "!x{([A-Z][a-z]+\\n)+}";
  std::string document = "Anna\nJuan\nPaul\nLuis\n";
  std::vector<DummyMapping> expected_matches = {
    DummyMapping({{"x", {0, 5}}}),
    DummyMapping({{"x", {5, 10}}}),
    DummyMapping({{"x", {10, 15}}}),
    DummyMapping({{"x", {15, 20}}})
  };

  REMatch::Regex regex = REMatch::compile(pattern);
  Flags flags = Flags(1, Flags::LineByLine);
  REMatch::MatchIterator match_iterator = regex.finditer(document, flags);

  run_client_test(match_iterator, expected_matches);
}

}
