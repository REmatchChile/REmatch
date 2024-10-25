# ##############################################################################
# Define target
# ##############################################################################
set(LIBRARY_TARGET_NAME REmatch)

file(GLOB_RECURSE ${LIBRARY_TARGET_NAME}_SRC src/*.cpp)
file(GLOB_RECURSE ${LIBRARY_TARGET_NAME}_HDR include/REmatch/*.hpp)

add_library(
  ${LIBRARY_TARGET_NAME}
  ${${LIBRARY_TARGET_NAME}_SRC} ${${LIBRARY_TARGET_NAME}_HDR} ${REMATCH_SRC}
  ${REMATCH_HDR})

add_library(${PROJECT_NAME}::${LIBRARY_TARGET_NAME} ALIAS
            ${LIBRARY_TARGET_NAME})

# ##############################################################################
# Define target properties, flags and dependencies
# ##############################################################################
generate_export_header(${LIBRARY_TARGET_NAME} EXPORT_FILE_NAME
  ${CMAKE_CURRENT_SOURCE_DIR}/include/${LIBRARY_TARGET_NAME}/${LIBRARY_TARGET_NAME}_export.hpp
)

set_target_properties(
  ${LIBRARY_TARGET_NAME}
  PROPERTIES DEBUG_POSTFIX "-d" # For MSVC
             MSVC_RUNTIME_LIBRARY "${MSVC_RUNTIME_TYPE}"
             VERSION "${CMAKE_PROJECT_VERSION}"
             SOVERSION "${CMAKE_PROJECT_VERSION}"
             PUBLIC_HEADER "${${LIBRARY_TARGET_NAME}_HDR}")

target_compile_options(${LIBRARY_TARGET_NAME} PRIVATE ${COMPILER_OPTIONS})

target_compile_definitions(${LIBRARY_TARGET_NAME}
                           PRIVATE ${COMPILER_DEFINITIONS})

target_link_options(${LIBRARY_TARGET_NAME} PRIVATE ${LINKER_FLAGS})

target_include_directories(${LIBRARY_TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(${LIBRARY_TARGET_NAME} PRIVATE antlr4_static)

# ##############################################################################
# Library install
# ##############################################################################
if(NOT SKBUILD AND NOT EMSCRIPTEN)
  install(
    TARGETS ${LIBRARY_TARGET_NAME}
    EXPORT ${PROJECT_NAME}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}                            COMPONENT shlib
    ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}                            COMPONENT lib
    RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}                            COMPONENT bin
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME} COMPONENT dev)
endif()
