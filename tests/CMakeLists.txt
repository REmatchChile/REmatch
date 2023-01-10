
message("Adding testing configuration")

find_package(Catch2 3 REQUIRED)

add_compile_definitions(private=public)

# Set testing binary filename
set(TEST_BINARY rematch_test)

set(TEST_SOURCES
        structs/garbage_collector.cpp
        structs/ecs_node.cpp
        test_searching.cpp
        test_searching.hpp test_segments.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${TEST_BINARY} ${TEST_SOURCES})

target_link_libraries(${TEST_BINARY} PUBLIC ${PROJECT_NAME}lib PRIVATE Catch2::Catch2WithMain)

include(Catch)

catch_discover_tests(${TEST_BINARY})
