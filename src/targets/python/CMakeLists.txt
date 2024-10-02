message(STATUS "Building python target")

# Install pybind11
include("${PROJECT_SOURCE_DIR}/cmake/setup_pybind11.cmake")

# Add python binding module
pybind11_add_module(_pyrematch MODULE ${CMAKE_CURRENT_SOURCE_DIR}/src/targets/python/python_binding.cpp)
target_link_libraries(_pyrematch PRIVATE REmatch-lib antlr4_cpp_runtime)

install(TARGETS _pyrematch DESTINATION .)
