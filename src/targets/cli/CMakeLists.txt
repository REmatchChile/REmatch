set(CLI_TARGET_NAME rematch-cli)

set(${CLI_TARGET_NAME}_SRC
    rematch-cli.cpp
)

add_executable(${CLI_TARGET_NAME} ${${CLI_TARGET_NAME}_SRC})

target_link_libraries(${CLI_TARGET_NAME} PRIVATE REmatch::REmatch)

install(TARGETS ${CLI_TARGET_NAME}
        RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT bin)
