# ##############################################################################
# Define target
# ##############################################################################
set(CLI_TARGET_NAME rematch-cli)

set(${CLI_TARGET_NAME}_SRC
    rematch-cli.cpp
)

add_executable(${CLI_TARGET_NAME} ${${CLI_TARGET_NAME}_SRC})

# ##############################################################################
# Define target properties, flags and dependencies
# ##############################################################################
set_target_properties(${CLI_TARGET_NAME} PROPERTIES
    DEBUG_POSTFIX "-d"
    MSVC_RUNTIME_LIBRARY "${MSVC_RUNTIME_TYPE}"
)

target_compile_options(${CLI_TARGET_NAME} PRIVATE ${COMPILER_OPTIONS})

target_compile_definitions(${CLI_TARGET_NAME} PRIVATE ${COMPILER_DEFINITIONS})

target_link_options(${CLI_TARGET_NAME} PRIVATE ${LINKER_FLAGS})

target_link_libraries(${CLI_TARGET_NAME} PRIVATE REmatch)

# ##############################################################################
# Executable install
# ##############################################################################
if (NOT SKBUILD AND NOT EMSCRIPTEN)
    install(
      TARGETS ${CLI_TARGET_NAME}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
    )
endif()