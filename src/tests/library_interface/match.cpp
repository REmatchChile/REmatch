#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <memory>
#include <sstream>

#include "evaluation/document.hpp"
#include "mediator/mapping.hpp"
#include "parsing/parser.hpp"

#include <REmatch/REmatch.hpp>

namespace REmatch::testing {
using namespace REmatch::library_interface;

std::string get_group_dict_info(const std::map<std::string, Span>& group_dict) {
  std::ostringstream stream;
  for (auto& pair : group_dict) {
    stream << pair.first << " -> |" << pair.second.first << ", " << pair.second.second << ">\n";
  }
  return stream.str();
}

Match construct_match(const std::string& document_, const std::string& query,
                      std::unique_ptr<mediator::Mapping> mapping) {
  auto parser = Parser(query);
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();
  auto document = std::make_shared<Document>(document_);
  return Match(std::move(mapping), variable_catalog, document);
}

TEST_CASE("match object returns the correct span indexes") {
  std::string document = "aaa";
  std::string regex = "!x{a}";

  std::map<std::string, Span> spans_map = {{"x", {1, 2}}};
  auto mapping = std::make_unique<mediator::Mapping>(std::move(spans_map));
  Match match = construct_match(document, regex, std::move(mapping));

  REQUIRE(match.start(0) == 1);
  REQUIRE(match.end(0) == 2);

  REQUIRE(match.start("x") == 1);
  REQUIRE(match.end("x") == 2);
}

TEST_CASE("match object returns the correct span object") {
  std::string document = "aba";
  std::string regex = "!x{b}";

  std::map<std::string, Span> spans_map = {{"x", {1, 2}}};
  auto mapping = std::make_unique<mediator::Mapping>(std::move(spans_map));

  Match match = construct_match(document, regex, std::move(mapping));

  REQUIRE(match.span(0).first == 1);
  REQUIRE(match.span(0).second == 2);

  REQUIRE(match.span("x").first == 1);
  REQUIRE(match.span("x").second == 2);
}

TEST_CASE("match object returns the correct groups") {
  std::string document_ = "aaaabbbb";
  auto document = std::make_shared<Document>(document_);
  std::string regex = "!x{a+}!y{b+}";

  auto parser = Parser(regex);
  std::shared_ptr<VariableCatalog> variable_catalog =
      parser.get_variable_catalog();

  std::map<std::string, Span> spans_map = {{"x", {0, 4}}, {"y", {4, 8}}};
  auto mapping = std::make_unique<mediator::Mapping>(std::move(spans_map));
  Match match{std::move(mapping), variable_catalog, document};

  REQUIRE(match.group(0) == "aaaa");
  REQUIRE(match.group(1) == "bbbb");

  REQUIRE(match.group("x") == "aaaa");
  REQUIRE(match.group("y") == "bbbb");
}

TEST_CASE("match object returns the correct group dictionary") {
  std::string document = "aaaabbbb";
  std::string regex = "!x{a+}!y{b+}";

  std::map<std::string, Span> mapping_dict = {{"x", {0, 4}}, {"y", {4, 8}}};
  std::map<std::string, Span> spans_map = {{"x", {0, 4}}, {"y", {4, 8}}};
  auto mapping = std::make_unique<mediator::Mapping>(std::move(spans_map));
  Match match = construct_match(document, regex, std::move(mapping));

  std::map<std::string, Span> group_dict = match.groupdict();
  INFO(get_group_dict_info(group_dict));
  REQUIRE(group_dict == mapping_dict);
}

TEST_CASE("match object returns the variables in the mapping") {
  std::string document = "abc";
  std::string regex = "!x{!z{a}b}!y{c}";

  std::map<std::string, Span> spans_map = {
      {"x", {0, 2}}, {"y", {2, 3}}, {"z", {0, 1}}};
  auto mapping = std::make_unique<mediator::Mapping>(std::move(spans_map));
  Match match = construct_match(document, regex, std::move(mapping));
  REQUIRE(match.variables() == std::vector<std::string>{"x", "y", "z"});
}

TEST_CASE("match object returns an empty vector when there are no variables") {
  std::string document = "aa";
  std::string regex = "a";

  auto mapping = std::unique_ptr<mediator::Mapping>();
  Match match = construct_match(document, regex, std::move(mapping));
  REQUIRE(match.variables().empty());
}

TEST_CASE("match object returns empty when the mapping is empty") {
  std::string document = "aa";
  std::string regex = "a";

  auto mapping = std::make_unique<mediator::Mapping>();
  Match match = construct_match(document, regex, std::move(mapping));
  REQUIRE(match.empty());
}

}  // namespace REmatch::testing
