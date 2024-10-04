find_package(Catch2 3 REQUIRED)

# Set testing binary filename
set(TEST_BINARY rematch-test)

file(GLOB_RECURSE TEST_SRC *.cpp)

include_directories(.)
add_executable(${TEST_BINARY} ${TEST_SRC} ${COMMON_SRC} ${COMMON_HDR})
message(STATUS "COMMON: ${COMMON_SRC} ${COMMON_HDR}")

target_link_libraries(${TEST_BINARY} PRIVATE REmatch::REmatch Catch2::Catch2WithMain antlr4_cpp_runtime)

include(CTest)
include(Catch)

catch_discover_tests(${TEST_BINARY})
