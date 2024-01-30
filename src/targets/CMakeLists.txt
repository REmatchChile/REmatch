set(BUILD_TARGETS
  rematch
  rematch_matches
  rematch_count
  rematch_stats
)

if(EMSCRIPTEN)
  message("Building emscripten target")
  add_executable(emscripten_binding ${CMAKE_CURRENT_SOURCE_DIR}/main/emscripten_binding.cpp ${sources})
  target_link_libraries(emscripten_binding ${project_LIB})
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

elseif(SKBUILD)
  message(STATUS "Building python target")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCMAKE_POSITION_INDEPENDENT_CODE=ON")
  # find_package(PythonLibs REQUIRED)
  # include_directories(${PYTHON_INCLUDE_DIRS})
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  # find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
  find_package(PythonLibs REQUIRED)
  include_directories(${PYTHON_INCLUDE_DIRS})

  message("Python version: ${PYTHON_VERSION_STRING}")

  find_package(pybind11 CONFIG REQUIRED)
  pybind11_add_module(_pyrematch MODULE ${CMAKE_CURRENT_SOURCE_DIR}/main/python_binding.cpp)
  set_target_properties(_pyrematch PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
  # target_link_libraries(_pyrematch PRIVATE ${project_LIB})
  target_link_libraries(_pyrematch PRIVATE ${project_LIB} ${PYTHON_LIBRARIES})

  install(TARGETS _pyrematch DESTINATION .)

else()
  message(STATUS "Building REmatch targets")
  foreach(target ${BUILD_TARGETS})
    add_executable(${target} ${CMAKE_CURRENT_SOURCE_DIR}/main/${target}.cpp ${sources})
    target_link_libraries(${target} PRIVATE ${project_LIB})
  endforeach()
endif()
