message("Building JavaScript/WASM target")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwasm-exceptions")
add_executable(rematch-bindings ${CMAKE_CURRENT_SOURCE_DIR}/src/targets/javascript/emscripten_binding.cpp ${COMMON_SRCS})
target_link_libraries(rematch-bindings PRIVATE REmatch-lib antlr4_cpp_runtime)
set_target_properties(rematch-bindings PROPERTIES LINK_FLAGS
"-lembind \
-fwasm-exceptions \
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