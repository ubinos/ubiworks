include("${CMAKE_CURRENT_LIST_DIR}/common_python.cmake")

add_custom_target(load
)

set(_python_run_cmd python -u ${PROJECT_PYTHON_MAIN} ${PROJECT_PYTHON_MAIN_OPTION})
message(STATUS "Run Command: ${_python_run_cmd}")
add_custom_target(run
    WORKING_DIRECTORY ${PROJECT_PYTHON_MAIN_DIR}
    COMMAND ${_python_run_cmd}
    USES_TERMINAL
    VERBATIM
)

set(_python_debug_cmd python -u ${PROJECT_PYTHON_MAIN} --debug ${PROJECT_PYTHON_MAIN_OPTION})
message(STATUS "Debug Command: ${_python_debug_cmd}")
add_custom_target(debug
    WORKING_DIRECTORY ${PROJECT_PYTHON_MAIN_DIR}
    COMMAND ${_python_run_cmd}
    USES_TERMINAL
    VERBATIM
)
