# src: https://github.com/adeharo9/antlr4-cpp-runtime/tree/master
set(ANTLR4_TARGET_NAME antlr4_static)

file(GLOB_RECURSE ANTLR4_SRC src/*.cpp)
file(GLOB_RECURSE ANTLR4_HDR src/*.hpp)

# Embed ANTLR4 as an object library to be used by other projects
add_library(${ANTLR4_TARGET_NAME} OBJECT ${ANTLR4_SRC} ${ANTLR4_HDR})

target_include_directories(${ANTLR4_TARGET_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(${ANTLR4_TARGET_NAME} PUBLIC ANTLR4CPP_STATIC)

install(TARGETS ${ANTLR4_TARGET_NAME}
        EXPORT  ${PROJECT_NAME}
)