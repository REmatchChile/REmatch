file(GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)

foreach(item ${project_SRCS})
  if(${item} MATCHES "main*")
    list(REMOVE_ITEM project_SRCS ${item})
  endif()
endforeach()

if(SWIG) # Only set shared library target

  set(project_LIB "${PROJECT_NAME}swiglib")
  set_property(SOURCE interfaces/rematch.swg PROPERTY CPLUSPLUS ON)
  swig_add_library(${project_LIB} TYPE SHARED LANGUAGE python SOURCES interfaces/rematch.swg ${project_SRCS})
  set_property(TARGET ${project_LIB} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SWIG_OUTDIR})
  target_link_libraries(${project_LIB} PRIVATE Python3::Python)

else()
  set (project_LIB ${PROJECT_NAME}lib)

  add_library(${project_LIB} ${project_SRCS})
  set_property (TARGET ${project_LIB} PROPERTY CXX_STANDARD 14)
  target_link_libraries (${project_LIB} ${project_LIBS})
  list (APPEND project_LIBS ${project_LIB})

  if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(rematch_BIN "rematch_wasm")
    add_executable(${rematch_BIN} interfaces/wasminterface.cpp)
  else()
    set(rematch_BIN "rematch")
    add_executable(${rematch_BIN} main.cpp)
    target_link_libraries(${rematch_BIN} ${project_LIBS})
    set_property(TARGET ${rematch_BIN} PROPERTY CXX_STANDARD 14)
    set(resample_BIN "resample")
  endif()

  target_link_libraries(${rematch_BIN} ${project_LIBS})
  set_property(TARGET ${rematch_BIN} PROPERTY CXX_STANDARD 14)

endif()


