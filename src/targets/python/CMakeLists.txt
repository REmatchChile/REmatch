set(PYTHON_TARGET_NAME _pyrematch)

set(${PYTHON_TARGET_NAME}_SRC
    python_binding.cpp
)

# Install pybind11
include(${PROJECT_SOURCE_DIR}/cmake/setup_pybind11.cmake)

# Add python binding module
pybind11_add_module(${PYTHON_TARGET_NAME} MODULE ${${PYTHON_TARGET_NAME}_SRC})
target_link_libraries(${PYTHON_TARGET_NAME} PRIVATE REmatch::REmatch)

install(TARGETS ${PYTHON_TARGET_NAME} LIBRARY DESTINATION . COMPONENT shlib)
