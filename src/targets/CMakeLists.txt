include_directories(main)

set(sources
)

set(BUILD_TARGETS
  rematch
  rematch_matches
  rematch_count
)

set(BENCHMARK_TARGETS
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
