include_directories(main)

set(sources
  main/interface.cpp
  main/interface.hpp
)

set(BUILD_TARGETS
  rematch
  test-ecs
  test-mempool
)

set(BENCHMARK_TARGETS
  test-onig
  test-hscan
  test-pcre
  test-re2
  test-boost
  test-tre
  force-all-re2
  force-all-re2-timeout
  # force-all-re2-2vars
  force-all-boost
  force-all-pcre
  force-suffix-re2
)

set(REGEX_LIBS
  pcrecpp
  onig
  hs
  re2
  boost_regex
  tre
)

message("REmatch libs: ${REMATCH_LIBS}")

foreach(target ${BUILD_TARGETS})
  add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/main/${target}.cpp ${sources})
  target_link_libraries(${target} ${REMATCH_LIBS})
endforeach(target ${BUILD_TARGETS})

if(REMATCH_BUILD_BENCHMARKS)
  foreach(target ${BENCHMARK_TARGETS})
    add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/${target}.cpp)
    target_link_libraries(${target} ${REMATCH_LIBS} ${REGEX_LIBS})
  endforeach(target ${BENCHMARK_TARGETS})
endif()