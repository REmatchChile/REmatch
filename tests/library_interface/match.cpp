#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include <memory>
#include "mediator/mediator.hpp"
#include "library_interface/match.hpp"

namespace rematch::testing {
using namespace REMatch::library_interface;

std::string get_group_dict_info(std::map<std::string, Span> group_dict);
Match construct_match(std::string& document, std::string regex,
                           rematch::mediator::Mapping mapping);

TEST_CASE("match object returns the correct span indexes") {
  std::string document = "aaa";
  std::string regex = "!x{a}";

  auto mapping = rematch::mediator::Mapping({{"x", {1, 2}}});
  Match match = construct_match(document, regex, mapping);

  REQUIRE(match.start(0) == 1);
  REQUIRE(match.end(0) == 2);

  REQUIRE(match.start("x") == 1);
  REQUIRE(match.end("x") == 2);
}

TEST_CASE("match object returns the correct span object") {
  std::string document = "aba";
  std::string regex = "!x{b}";

  auto mapping = rematch::mediator::Mapping({{"x", {1, 2}}});
  Match match = construct_match(document, regex, mapping);

  REQUIRE(match.span(0).first == 1);
  REQUIRE(match.span(0).second == 2);

  REQUIRE(match.span("x").first == 1);
  REQUIRE(match.span("x").second == 2);
}

TEST_CASE("match object returns the correct groups") {
  std::string document = "aaaabbbb";
  std::string regex = "!x{a+}!y{b+}";

  auto parser = Parser(regex);
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();
  LogicalVA logical_va = parser.get_logical_va();
  auto extended_va = ExtendedVA(logical_va);
  auto segment_manager_creator = SegmentManagerCreator(logical_va);
  auto mapping = rematch::mediator::Mapping({{"x", {0, 4}}, {"y", {4, 8}}});
  auto match = Match(mapping, variable_catalog, document);

  REQUIRE(match.group(0) == "aaaa");
  REQUIRE(match.group(1) == "bbbb");

  REQUIRE(match.group("x") == "aaaa");
  REQUIRE(match.group("y") == "bbbb");
}

TEST_CASE("match object returns the correct group dictionary") {
  std::string document = "aaaabbbb";
  document = START_CHAR + document + END_CHAR;
  std::string regex = "!x{a+}!y{b+}";

  std::map<std::string, Span> mapping_dict = {{"x", {0, 4}}, {"y", {4, 8}}};
  auto mapping = rematch::mediator::Mapping({{"x", {0, 4}}, {"y", {4, 8}}});
  Match match = construct_match(document, regex, mapping);

  std::map<std::string, Span> group_dict = match.groupdict();
  INFO(get_group_dict_info(group_dict));
  REQUIRE(group_dict == mapping_dict);
}

std::string get_group_dict_info(std::map<std::string, Span> group_dict) {
  std::ostringstream stream;
  for (auto& pair : group_dict) {
    stream << pair.first << " -> " << pair.second << std::endl;
  }
  return stream.str();
}

TEST_CASE("match object returns the variables in the mapping") {
  std::string document = "abc";
  std::string regex = "!x{!z{a}b}!y{c}";

  auto mapping = rematch::mediator::Mapping({{"x", {0, 2}}, {"y", {2, 3}}, {"z", {0, 1}}});
  Match match = construct_match(document, regex, mapping);
  REQUIRE(match.variables() == std::vector<std::string>{"x", "y", "z"});
}

TEST_CASE("match object returns an empty vector when there are no variables") {
  std::string document = "aa";
  std::string regex = "a";

  auto mapping = rematch::mediator::Mapping();
  Match match = construct_match(document, regex, mapping);
  REQUIRE(match.variables() == std::vector<std::string>{});
}

TEST_CASE("match object returns empty when the mapping is empty") {
  std::string document = "aa";
  std::string regex = "a";

  auto mapping = rematch::mediator::Mapping();
  Match match = construct_match(document, regex, mapping);
  REQUIRE(match.empty());
}

Match construct_match(std::string& document, std::string regex,
                           rematch::mediator::Mapping mapping) {
  auto parser = Parser(regex);
  std::shared_ptr<VariableCatalog> variable_catalog = parser.get_variable_catalog();
  LogicalVA logical_va = parser.get_logical_va();
  ExtendedVA extended_va = ExtendedVA(logical_va);
  auto segment_manager_creator = SegmentManagerCreator(logical_va);
  return Match(mapping, variable_catalog, document);
}

}  // namespace rematch::testing
