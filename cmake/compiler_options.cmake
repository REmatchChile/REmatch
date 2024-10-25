# Set default build type as Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Build type" FORCE)
  message(STATUS "Build type not specified. Using Release by default.")
else()
  message(STATUS "Using ${CMAKE_BUILD_TYPE} configuration")
endif()

# Define compiler options
if(MSVC)
  list(APPEND COMPILER_OPTIONS
    /W4
    /permissive-
    $<$<CONFIG:RELEASE>:/O2 /Ob2 /GL>
    $<$<CONFIG:MINSIZEREL>:/O1 /Ob1 /GL>
    $<$<CONFIG:RELWITHDEBINFO>:/O2 /Zi /Ob1>
    $<$<CONFIG:DEBUG>:/Od /Zi /Ob0 /RTC1>
  )

  list(APPEND COMPILER_DEFINITIONS
    _UNICODE
    WINDOWS
    $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:RELWITHDEBINFO>,$<CONFIG:MINSIZEREL>>:NDEBUG>
    $<$<CONFIG:DEBUG>:_DEBUG>
  )

  list(APPEND LINKER_FLAGS
    $<$<BOOL:${BUILD_SHARED_LIBS}>:/LTCG>
  )

  set(MSVC_RUNTIME_TYPE $<IF:$<BOOL:${BUILD_WITH_MT}>,MultiThreaded$<$<CONFIG:Debug>:Debug>,MultiThreaded$<$<CONFIG:Debug>:Debug>>DLL)
else()
  list(APPEND COMPILER_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
    -funroll-loops
    -fno-operator-names
    $<$<CONFIG:RELEASE>:-O3>
    $<$<CONFIG:DEBUG>:-O0 -g3>
    )

  # EMSCRIPTEN doesn't support -march=native flag
  if (NOT EMSCRIPTEN)
    list(APPEND COMPILER_OPTIONS
      -march=native
    )
  endif()

  list(APPEND COMPILER_DEFINITIONS
    $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:RELWITHDEBINFO>,$<CONFIG:MINSIZEREL>>:NDEBUG>
  )

  list(APPEND LINKER_FLAGS
    $<$<NOT:$<CXX_COMPILER_ID:AppleClang>>:-Wl,-z,defs>
    $<$<NOT:$<CXX_COMPILER_ID:AppleClang>>:-Wl,-z,now>
    $<$<NOT:$<CXX_COMPILER_ID:AppleClang>>:-Wl,-z,relro>
    # Clang doesn't support these hardening flags
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-Wl,-pie>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-fpie>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-pipe>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>,$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>>:-static-libstdc++>
    $<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>
    $<$<CONFIG:DEBUG>:-fsanitize=address>
    $<$<CONFIG:DEBUG>:-fsanitize=leak>
    $<$<CONFIG:DEBUG>:-fsanitize=undefined>
    $<$<AND:$<CONFIG:DEBUG>,$<BOOL:${BUILD_SHARED_LIBS}>>:-shared-libasan>  # Link ASan runtime
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>>:-fstack-clash-protection>
    $<$<AND:$<NOT:$<CXX_COMPILER_ID:AppleClang>>,$<NOT:$<CXX_COMPILER_ID:Clang>>>:-fbounds-check>
    -fstack-protector
    $<$<BOOL:${CMAKE_POSITION_INDEPENDENT_CODE}>:-fpic>
  )
endif()