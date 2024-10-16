add_subdirectory(lib)

if(SKBUILD)
  add_subdirectory(python)
elseif(EMSCRIPTEN)
  add_subdirectory(javascript)
else()
  add_subdirectory(cli)
endif()

# if(NOT SKBUILD) endif()

# install(TARGETS ${CLI_TARGET_NAME}
# RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT bin)
