file (GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  foreach(item ${project_SRCS})
    if(${item} MATCHES "main*")
      list(REMOVE_ITEM project_SRCS ${item})
    endif()
  endforeach()
endif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")

set (project_LIB ${PROJECT_NAME}lib)

add_library (${project_LIB} ${project_SRCS})
set_property (TARGET ${project_LIB} PROPERTY CXX_STANDARD 11)
target_link_libraries (${project_LIB} ${project_LIBS})
list (APPEND project_LIBS ${project_LIB})


if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set (rematch_BIN "spanners_interface")
    add_executable (${rematch_BIN} wasminterface.cpp)
else()
    set (rematch_BIN "rematch")
    add_executable (${rematch_BIN} main.cpp)
    set (resample_BIN "resample")
    add_executable(${resample_BIN} maingen.cpp)
    target_link_libraries(${resample_BIN} ${project_LIBS})
endif()

target_link_libraries(${rematch_BIN} ${project_LIBS})

set_property (TARGET ${rematch_BIN} PROPERTY CXX_STANDARD 11)