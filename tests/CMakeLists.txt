find_package(Catch2 3 REQUIRED)

add_compile_definitions(private=public)

# Set testing binary filename
set(TEST_BINARY rematch_test)

set(TEST_SOURCES
  output_enumeration/garbage_collector.cpp
  output_enumeration/ecs_node.cpp
  output_enumeration/ecs.cpp
  output_enumeration/enumerator.cpp
  output_enumeration/mapping.cpp
  output_enumeration/extended_mapping.cpp
  parsing/parser.cpp
  parsing/logical_va.cpp
  filtering_module/segment_identification.cpp
  filtering_module/search_nfa.cpp
  filtering_module/search_dfa.cpp
  evaluation/extended_va.cpp
  evaluation/extended_det_va.cpp
  evaluation/dummy_mapping.hpp
  evaluation/mapping_helpers.hpp
  evaluation/finditer_algorithm.cpp
  evaluation/findone_algorithm.cpp
  evaluation/algorithm_utf8.cpp
  mediator/finditer_mediator.cpp
  mediator/findone_mediator.cpp
  mediator/multi_finditer_mediator.cpp
  mediator/multi_findone_mediator.cpp
  mediator/segment_manager_creator.cpp
  library_interface/match.cpp
  library_interface/multi_match.cpp
  library_interface/match_iterator.cpp
  library_interface/multi_match_iterator.cpp
  library_interface/query.cpp
  library_interface/multi_query.cpp
  library_interface/rematch_compile.cpp
  library_interface/rematch_functions.cpp
  library_interface/flags.cpp
  exceptions/anchor_inside_capture_exception.cpp
  exceptions/complex_query_exception.cpp
  exceptions/empty_word_capture_exception.cpp
  exceptions/invalid_escape_exception.cpp
  exceptions/invalid_range_exception.cpp
  exceptions/memory_limit_exceeded_exception.cpp
  exceptions/query_syntax_exception.cpp
  exceptions/same_nested_variable_exception.cpp
  exceptions/variable_not_found_exception.cpp
  exceptions/variable_not_found_in_catalog_exception.cpp
  exceptions/multi_spanners_not_allowed_exception.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_link_libraries(${TEST_BINARY} PUBLIC ${project_LIB} PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)

catch_discover_tests(${TEST_BINARY})
