find_package(Catch2 3 REQUIRED)

add_compile_definitions(private=public)

# Set testing binary filename
set(TEST_BINARY rematch_test)

file(GLOB_RECURSE TEST_SRCS ${CMAKE_SOURCE_DIR}/tests/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_executable(${TEST_BINARY} ${TEST_SRCS})

target_link_libraries(${TEST_BINARY} PUBLIC REmatch-lib PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)

catch_discover_tests(${TEST_BINARY})
