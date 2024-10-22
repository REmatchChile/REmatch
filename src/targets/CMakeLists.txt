add_subdirectory(lib)

if(SKBUILD)
  add_subdirectory(python)
elseif(EMSCRIPTEN)
  add_subdirectory(javascript)
else()
  add_subdirectory(cli)
endif()
