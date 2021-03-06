file(GLOB sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/automata/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/det/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/factories/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/visitors/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/parser_automata/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/regex/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/structs/*.cpp
)

set(BUILD_TARGETS
  rematch
  # generate
  test-regex
  test-anchors
)

if(SWIG) # Only set shared library target
  set(project_LIB "${PROJECT_NAME}")
  set_property(SOURCE interfaces/rematch.i PROPERTY CPLUSPLUS ON)
  swig_add_library(${project_LIB} TYPE SHARED LANGUAGE python SOURCES interfaces/rematch.i ${sources})
  set_property(TARGET ${project_LIB} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SWIG_OUTDIR})
  set_property(TARGET ${project_LIB} PROPERTY SWIG_COMPILE_OPTIONS -O)
  message("Output dir: ${CMAKE_SWIG_OUTDIR}")
  target_link_libraries(${project_LIB} PRIVATE Python3::Python)
else()
  set(project_LIB ${PROJECT_NAME}lib)

  add_library(${project_LIB} ${sources})
  set_property (TARGET ${project_LIB} PROPERTY CXX_STANDARD 17)
  target_link_libraries (${project_LIB} ${project_LIBS})
  list(APPEND project_LIBS ${project_LIB})

  if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(target "rematch_wasm")
    add_executable(${target} interfaces/wasm.cpp)
    target_link_libraries(${target} ${project_LIBS})
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
	set_target_properties(${target} PROPERTIES LINK_FLAGS "--bind -s WASM_ASYNC_COMPILATION=0 -s ALLOW_MEMORY_GROWTH=1")
  else()
    foreach(target ${BUILD_TARGETS})
      add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/main/${target}.cpp)
      target_link_libraries(${target} ${project_LIBS})
      set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
    endforeach(target ${BUILD_TARGETS})
  endif()

endif()


