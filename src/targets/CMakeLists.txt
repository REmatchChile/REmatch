set(BUILD_TARGETS
  rematch
  rematch_matches
  rematch_count
  rematch_stats
)

if(EMSCRIPTEN)
  message("Building emscripten target")
  add_executable(emscripten_binding ${CMAKE_CURRENT_SOURCE_DIR}/main/emscripten_binding.cpp ${sources})
  target_link_libraries(emscripten_binding ${PROJECT_NAME}lib)
  set_target_properties(emscripten_binding PROPERTIES LINK_FLAGS
    "-lembind \
    -fwasm-exceptions \
    -sMODULARIZE=1 \
    -sENVIRONMENT=web \
    -sEXPORT_NAME=REmatch \
    -sEXPORT_ES6=1 \
    -sSINGLE_FILE=1 \
    -sINITIAL_MEMORY=512MB \
    -sALLOW_MEMORY_GROWTH=1 \
    -sMEMORY_GROWTH_GEOMETRIC_STEP=1.0 \
    -sMEMORY_GROWTH_GEOMETRIC_CAP=0 \
    -sEXPORT_EXCEPTION_HANDLING_HELPERS=1"
  )
  return()
else()
  foreach(target ${BUILD_TARGETS})
    add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/main/${target}.cpp ${sources})
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME}lib)
  endforeach()
endif()
