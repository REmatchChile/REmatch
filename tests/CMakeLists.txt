# Set testing binary filename
set(BINARY rematch_test)

set(TEST_SOURCES
        test_searching.hpp
        test_searching.cpp
        data/search.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# NOTE: Maybe use add_library instead if too many source files
add_executable(${BINARY} ${TEST_SOURCES})

target_link_libraries(${BINARY} PUBLIC ${PROJECT_NAME}lib)