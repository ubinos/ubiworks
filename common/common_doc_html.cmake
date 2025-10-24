add_custom_target(xdserver
)

add_custom_target(load
)

add_custom_target(rsync
)

add_custom_target(build
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND sphinx-build -M html "${PROJECT_DOC_DIR}" .
    VERBATIM
)

add_custom_target(rebuild
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target build
    VERBATIM
)

##
set(_doc_file_html "${CMAKE_BINARY_DIR}/html/index.html")

if(WIN32)
    set(_open_cmd_html start "" "${_doc_file_html}")
elseif(APPLE)
    set(_open_cmd_html open "${_doc_file_html}")
else()
    set(_open_cmd_html xdg-open "${_doc_file_html}")
endif()

add_custom_target(run
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${_open_cmd_html}
    VERBATIM
)
