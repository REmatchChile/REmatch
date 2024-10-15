add_subdirectory(targets)

if(BUILD_TESTING AND NOT SKBUILD AND NOT EMSCRIPTEN)
    add_subdirectory(tests)
endif()