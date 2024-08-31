set(BUILD_TARGETS
  rematch
)

if(EMSCRIPTEN)
  message("Building javascript target")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwasm-exceptions")
  add_executable(rematch-bindings ${CMAKE_CURRENT_SOURCE_DIR}/main/emscripten_binding.cpp ${sources})
  target_link_libraries(rematch-bindings PRIVATE ${project_LIB})
  set_target_properties(rematch-bindings PROPERTIES LINK_FLAGS
    "-lembind \
    -fwasm-exceptions \
    -sMODULARIZE=1 \
    -sSINGLE_FILE=1 \
    -sEXPORT_NAME=REmatchModule \
    -sEXPORT_ES6=1 \
    -sALLOW_MEMORY_GROWTH=1 \
    -sMEMORY_GROWTH_GEOMETRIC_STEP=1.0 \
    -sMEMORY_GROWTH_GEOMETRIC_CAP=0 \
    -sEXPORT_EXCEPTION_HANDLING_HELPERS=1 \
    -sWASM_BIGINT=1"
  )
  return()

elseif(SKBUILD)
  message(STATUS "Building python target")
  include(${PROJECT_SOURCE_DIR}/cmake/setup_pybind11.cmake)
  pybind11_add_module(_pyrematch MODULE ${CMAKE_CURRENT_SOURCE_DIR}/main/python_binding.cpp)
  target_link_libraries(_pyrematch PRIVATE ${project_LIB})
  install(TARGETS _pyrematch DESTINATION .)
else()
  message(STATUS "Building REmatch targets")
  foreach(target ${BUILD_TARGETS})
    add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/main/${target}.cpp ${sources})
    target_link_libraries(${target} PRIVATE ${project_LIB})
  endforeach()
endif()
