add_custom_target(xdserver
)

add_custom_target(load
)

add_custom_target(rsync
)

add_custom_target(build
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND sphinx-build -M html "${PROJECT_DOC_DIR}" .
    COMMAND sphinx-build -M latexpdf "${PROJECT_DOC_DIR}" .
    VERBATIM
)

add_custom_target(rebuild
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target build
    VERBATIM
)

##
set(_doc_file_html "${CMAKE_BINARY_DIR}/html/index.html")
if(NOT _doc_file_latexpdf)
    set(_doc_file_latexpdf "${CMAKE_BINARY_DIR}/latex/sphinx.pdf")
endif()

if(WIN32)
    set(_open_cmd_html start "" "${_doc_file_html}")
    set(_open_cmd_latexpdf start "" "${_doc_file_latexpdf}")
elseif(APPLE)
    set(_open_cmd_html open "${_doc_file_html}")
    set(_open_cmd_latexpdf open "${_doc_file_latexpdf}")
else()
    set(_open_cmd_html xdg-open "${_doc_file_html}")
    set(_open_cmd_latexpdf xdg-open "${_doc_file_latexpdf}")
endif()

add_custom_target(run
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${_open_cmd_html}
    COMMAND ${_open_cmd_latexpdf}
    VERBATIM
)
