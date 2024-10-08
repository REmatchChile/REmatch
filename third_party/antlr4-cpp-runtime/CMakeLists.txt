# src: https://github.com/adeharo9/antlr4-cpp-runtime/tree/master
set(ANTLR4_CPP_RUNTIME antlr4_cpp_runtime CACHE INTERNAL "")

file(GLOB_RECURSE ANTLR4_SRC src/*.cpp)
file(GLOB_RECURSE ANTLR4_HDR src/*.hpp)

set(ANTLR4_CPP_RUNTIME_SRC ${ANTLR4_SRC} ${ANTLR4_HDR})

add_library(${ANTLR4_CPP_RUNTIME} STATIC ${ANTLR4_CPP_RUNTIME_SRC})

target_include_directories(${ANTLR4_CPP_RUNTIME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(${ANTLR4_CPP_RUNTIME} PUBLIC ANTLR4CPP_STATIC)