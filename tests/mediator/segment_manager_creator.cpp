#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#undef private
#include "evaluation/document.hpp"
#include "parsing/parser.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "mediator/segment_manager/segment_manager_creator.hpp"
#include "mediator/segment_manager/line_by_line_manager.hpp"
#include "mediator/segment_manager/segment_identificator_manager.hpp"
#include "mediator/segment_manager/default_segment_manager.hpp"

namespace rematch::testing {

TEST_CASE(
    "the segment manager creator returns the correct type when there are no "
    "useful anchors") {
  auto parser = Parser("!x{a+}");
  auto document = std::make_shared<Document>("aaaaa");
  LogicalVA logical_va = parser.get_logical_va();

  auto segment_manager_creator = SegmentManagerCreator(logical_va);
  segment_manager_creator.set_document(document);
  auto segment_manager = segment_manager_creator.get_segment_manager();
  REQUIRE(dynamic_cast<const SegmentIdentificatorManager*>(segment_manager.get()) != nullptr);
}

TEST_CASE(
    "the segment manager creator returns the correct type when there are no "
    "useful anchors and flag line by line is true") {
  auto parser = Parser("!x{a+}");
  auto document = std::make_shared<Document>("aaaaa");
  LogicalVA logical_va = parser.get_logical_va();

  Flags flags = {true, false, 8, 1000};
  auto segment_manager_creator = SegmentManagerCreator(logical_va, flags);
  segment_manager_creator.set_document(document);
  auto segment_manager = segment_manager_creator.get_segment_manager();
  REQUIRE(dynamic_cast<const LineByLineManager*>(segment_manager.get()) != nullptr);
}

TEST_CASE(
    "the segment manager creator returns the correct type when there are "
    "useful anchors") {
  auto parser = Parser("^!x{a+}");
  auto document = std::make_shared<Document>("aaaaa");
  LogicalVA logical_va = parser.get_logical_va();

  Flags flags = {true, false, 8, 1000};
  auto segment_manager_creator = SegmentManagerCreator(logical_va, flags);
  segment_manager_creator.set_document(document);
  auto segment_manager = segment_manager_creator.get_segment_manager();
  REQUIRE(dynamic_cast<const DefaultSegmentManager*>(segment_manager.get()) != nullptr);
}

}  // namespace rematch::testing
