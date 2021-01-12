# Macro for listing all subirectories at current directory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# Set testing binary filename
set(BINARY rematch_test)

file(GLOB_RECURSE TEST_SOURCES *.hpp *.cpp)

# NOTE: Maybe use add_library instead if too many source files
add_executable(${BINARY} ${TEST_SOURCES})

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

foreach(subdir ${SUBDIRS})

    SUBDIRLIST(SUBSUBDIRS "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
    set(COUNT 0)

    foreach(subsubdir ${SUBSUBDIRS})

        add_test(NAME "${subdir}.${subsubdir}"
                 COMMAND ${BINARY}
                          --run_test=${subdir}/_${COUNT}
                          --catch_system_error=yes
                 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

        # Set timeout for tests
        set_tests_properties("${subdir}.${subsubdir}" PROPERTIES TIMEOUT 3)

        math(EXPR COUNT "${COUNT}+1")

    endforeach(subsubdir ${SUBSUBDIRS})
endforeach(subdir ${SUBDIRS})




target_link_libraries(${BINARY} PUBLIC ${PROJECT_NAME}lib)