#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <REmatch/REmatch.hpp>
#include "parsing/parser.hpp"
#include "parsing/variable_catalog.hpp"

namespace REmatch::testing {

TEST_CASE("accessing nonexistent variable raises an exception") {
  std::string regex = "!x{a}";
  auto parser = Parser(regex);
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();

  REQUIRE_THROWS_AS(variable_catalog->position("y"),
                    REmatch::VariableNotFoundInCatalogException);
}

}  // namespace REmatch::testing
