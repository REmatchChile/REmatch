set(BUILD_TARGETS
  rematch
  rematch_matches
  rematch_count
  rematch_stats
)

foreach(target ${BUILD_TARGETS})
  add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/main/${target}.cpp ${sources})
  target_link_libraries(${target} PRIVATE ${PROJECT_NAME}lib)
endforeach()
