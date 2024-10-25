if(BUILD_TESTING AND NOT SKBUILD AND NOT EMSCRIPTEN)
  find_package(Catch2 3 REQUIRED)

  # Set testing binary filename
  set(TEST_BINARY rematch-test)

  file(GLOB_RECURSE TEST_SRC *.cpp)
  file(GLOB_RECURSE TEST_HDR *.hpp)

  include_directories(.)
  add_executable(${TEST_BINARY} ${TEST_SRC} ${TEST_HDR} ${REMATCH_SRC} ${REMATCH_HDR})

  set_target_properties(${TEST_BINARY} PROPERTIES
    DEBUG_POSTFIX "-d" # For MSVC
    MSVC_RUNTIME_LIBRARY "${MSVC_RUNTIME_TYPE}"
  )

  target_compile_options(${TEST_BINARY} PRIVATE ${COMPILER_OPTIONS})

  target_compile_definitions(${TEST_BINARY} PRIVATE ${COMPILER_DEFINITIONS})

  target_link_options(${TEST_BINARY} PRIVATE ${LINKER_FLAGS})

  target_link_libraries(${TEST_BINARY} PRIVATE REmatch::REmatch Catch2::Catch2WithMain antlr4_static)

  include(CTest)
  include(Catch)

  catch_discover_tests(${TEST_BINARY} DISCOVERY_MODE  PRE_TEST)
endif()