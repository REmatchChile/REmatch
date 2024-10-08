set(LIBRARY_TARGET_NAME ${PROJECT_NAME})

# set(${LIBRARY_TARGET_NAME}_SRC
#     REmatch.cpp
# )

# set(${LIBRARY_TARGET_NAME}_HDR
#     REmatch.hpp
# )

file(GLOB_RECURSE ${LIBRARY_TARGET_NAME}_SRC src/*.cpp)

file(GLOB_RECURSE ${LIBRARY_TARGET_NAME}_HDR include/REmatch/*.hpp)

add_library(${LIBRARY_TARGET_NAME} ${${LIBRARY_TARGET_NAME}_SRC} ${${LIBRARY_TARGET_NAME}_HDR} ${COMMON_SRC} ${COMMON_HDR})

add_library(${PROJECT_NAME}::${LIBRARY_TARGET_NAME} ALIAS ${LIBRARY_TARGET_NAME})

set_target_properties(${LIBRARY_TARGET_NAME} PROPERTIES
                     VERSION ${${PROJECT_NAME}_VERSION}
                     PUBLIC_HEADER "${${LIBRARY_TARGET_NAME}_HDR}")

target_include_directories(${LIBRARY_TARGET_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

# SKBUILD just needs to have the libREmatch.a file available at the same directory as the package
if(SKBUILD)
    install(TARGETS ${LIBRARY_TARGET_NAME}
            EXPORT  ${PROJECT_NAME}
            LIBRARY DESTINATION . COMPONENT shlib
            ARCHIVE DESTINATION . COMPONENT lib)
else()
    install(TARGETS       ${LIBRARY_TARGET_NAME}
            EXPORT        ${PROJECT_NAME}
            LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}                            COMPONENT shlib
            ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}                            COMPONENT lib
            RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}                            COMPONENT bin
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME} COMPONENT dev)
endif()


message(STATUS "Created target ${LIBRARY_TARGET_NAME} for export ${PROJECT_NAME}.")