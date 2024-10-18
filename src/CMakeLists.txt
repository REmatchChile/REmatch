if(BUILD_TESTING AND NOT SKBUILD AND NOT EMSCRIPTEN)
  enable_testing()
endif()

add_subdirectory(targets)
add_subdirectory(tests)
