if(BUILD_TESTING AND NOT SKBUILD AND NOT EMSCRIPTEN)
  find_package(Catch2 3 REQUIRED)

  # Set testing binary filename
  set(TEST_BINARY rematch-test)

  file(GLOB_RECURSE TEST_SRC *.cpp)
  file(GLOB_RECURSE TEST_HDR *.hpp)

  include_directories(.)
  add_executable(${TEST_BINARY} ${TEST_SRC} ${TEST_HDR} ${REMATCH_SRC} ${REMATCH_HDR})

  target_link_libraries(${TEST_BINARY} PRIVATE REmatch::REmatch Catch2::Catch2WithMain antlr4_static)

  include(CTest)
  include(Catch)

  catch_discover_tests(${TEST_BINARY})

  install(TARGETS ${TEST_BINARY} RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT bin)
endif()