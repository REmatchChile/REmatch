set(JAVASCRIPT_TARGET_NAME rematch-bindings)

set(${JAVASCRIPT_TARGET_NAME}_SRC
    emscripten_binding.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwasm-exceptions")
add_executable(${JAVASCRIPT_TARGET_NAME} ${${JAVASCRIPT_TARGET_NAME}_SRC})
target_link_libraries(${JAVASCRIPT_TARGET_NAME} PRIVATE REmatch::REmatch)

set_target_properties(${JAVASCRIPT_TARGET_NAME} PROPERTIES
    MSVC_RUNTIME_LIBRARY "${MSVC_RUNTIME_TYPE}"
    LINK_FLAGS "-lembind \
                -fwasm-exceptions \
                -sFILESYSTEM=0 \
                -sASSERTIONS=0 \
                -sINVOKE_RUN=0 \
                -sMODULARIZE=1 \
                -sSINGLE_FILE=1 \
                -sEXPORT_NAME=REmatchModule \
                -sEXPORT_ES6=1 \
                -sALLOW_MEMORY_GROWTH=1 \
                -sMEMORY_GROWTH_GEOMETRIC_STEP=1.0 \
                -sMEMORY_GROWTH_GEOMETRIC_CAP=0 \
                -sEXPORT_EXCEPTION_HANDLING_HELPERS=1 \
                -sWASM_BIGINT=1"
)

target_compile_options(${JAVASCRIPT_TARGET_NAME} PRIVATE ${COMPILER_OPTIONS})

target_compile_definitions(${JAVASCRIPT_TARGET_NAME} PRIVATE ${COMPILER_DEFINITIONS})

install(TARGETS ${JAVASCRIPT_TARGET_NAME}
        RUNTIME DESTINATION COMPONENT bin)