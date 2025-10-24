include("${CMAKE_CURRENT_LIST_DIR}/common_python.cmake")

set(_python_rsync_cmd)
foreach(_sync_dir IN LISTS PROJECT_PYTHON_REMOTE_SYNC_DIRS)
    list(APPEND _python_rsync_cmd
        COMMAND rsync -av
            "${PROJECT_PYTHON_REMOTE_SOURCE_BASE}/${_sync_dir}/"
            "${PROJECT_PYTHON_REMOTE_USER}@${PROJECT_PYTHON_REMOTE_HOST}:${PROJECT_PYTHON_REMOTE_TARGET_BASE}/${_sync_dir}/"
    )
endforeach()

message(STATUS "Rsync Command: ${_python_rsync_cmd}")
add_custom_target(rsync
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    ${_python_rsync_cmd}
    VERBATIM
)

set(_python_run_cmd ssh -CYt "${PROJECT_PYTHON_REMOTE_USER}@${PROJECT_PYTHON_REMOTE_HOST}" "cd ${PROJECT_PYTHON_MAIN_DIR} && . ${PROJECT_PYTHON_REMOTE_VENV}/bin/activate && python -u ${PROJECT_PYTHON_MAIN} ${PROJECT_PYTHON_MAIN_OPTION}")
message(STATUS "Run Command: ${_python_run_cmd}")
add_custom_target(run
    WORKING_DIRECTORY ${PROJECT_PYTHON_MAIN_DIR}
    COMMAND ${_python_run_cmd}
    USES_TERMINAL
    VERBATIM
)

set(_python_debug_cmd ssh -CYt "${PROJECT_PYTHON_REMOTE_USER}@${PROJECT_PYTHON_REMOTE_HOST}" "cd ${PROJECT_PYTHON_MAIN_DIR} && . ${PROJECT_PYTHON_REMOTE_VENV}/bin/activate && python -u --debug ${PROJECT_PYTHON_MAIN} ${PROJECT_PYTHON_MAIN_OPTION}")
message(STATUS "Debug Command: ${_python_debug_cmd}")
add_custom_target(debug
    WORKING_DIRECTORY ${PROJECT_PYTHON_MAIN_DIR}
    COMMAND ${_python_run_cmd}
    USES_TERMINAL
    VERBATIM
)
