message(STATUS "UBI_CONFIG_WIFI_SSID:           ${UBI_CONFIG_WIFI_SSID}")
message(STATUS "UBI_CONFIG_WIFI_PW:             ${UBI_CONFIG_WIFI_PW}")
message(STATUS "UBI_CONFIG_SERVER_ADDR:         ${UBI_CONFIG_SERVER_ADDR}")

message(STATUS "PROJECT_TARGET_NAME:            ${PROJECT_TARGET_NAME}")
message(STATUS "PROJECT_BASE_DIR:               ${PROJECT_BASE_DIR}")
message(STATUS "PROJECT_LIB_DIR:                ${PROJECT_LIB_DIR}")
message(STATUS "PROJECT_BSP_DIR:                ${PROJECT_BSP_DIR}")
message(STATUS "PROJECT_SRC_DIR:                ${PROJECT_SRC_DIR}")

message(STATUS "CMAKE_EXPORT_COMPILE_COMMANDS:  ${CMAKE_EXPORT_COMPILE_COMMANDS}")
message(STATUS "CMAKE_BUILD_TYPE:               ${CMAKE_BUILD_TYPE}")

message(STATUS "CMAKE_CURRENT_BINARY_DIR:       ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_C_COMPILER_LAUNCHER:      ${CMAKE_C_COMPILER_LAUNCHER}")
message(STATUS "CMAKE_CXX_COMPILER_LAUNCHER:    ${CMAKE_CXX_COMPILER_LAUNCHER}")
message(STATUS "CMAKE_C_STANDARD:               ${CMAKE_C_STANDARD}")
message(STATUS "CMAKE_CXX_STANDARD:             ${CMAKE_CXX_STANDARD}")

file(RELATIVE_PATH _rel_path "${PROJECT_BASE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
execute_process(
    COMMAND ubitool json -w 
                         -k "[\"C_Cpp.default.compileCommands\"]"
                         -v "\${workspaceFolder}/${_rel_path}/compile_commands.json"
                         "${PROJECT_BASE_DIR}/.vscode/settings.json"
)
execute_process(
    COMMAND ubitool json -w 
                         -k "configurations[?name==\"local app debug\"].cwd | [0]"
                         -v "\${workspaceFolder}/${_rel_path}"
                         "${PROJECT_BASE_DIR}/.vscode/launch.json"
)
execute_process(
    COMMAND ubitool json -w 
                         -k "configurations[?name==\"local app debug\"].program | [0]"
                         -v "\${workspaceFolder}/${_rel_path}/app"
                         "${PROJECT_BASE_DIR}/.vscode/launch.json"
)
execute_process(
    COMMAND ubitool json -w 
                         -k "tasks[?label==\"target app reset\"].options.cwd | [0]"
                         -v "\${workspaceFolder}/${_rel_path}"
                         "${PROJECT_BASE_DIR}/.vscode/tasks.json"
)

add_custom_target(xdserver
)

add_custom_target(load
)

add_custom_target(rsync
)

add_custom_target(build
    COMMAND ${CMAKE_COMMAND} --build . --target all
    VERBATIM
)

add_custom_target(rebuild
    COMMAND ${CMAKE_COMMAND} --build . --target clean
    COMMAND ${CMAKE_COMMAND} --build . --target build
    VERBATIM
)

add_custom_target(run
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND "${PROJECT_TARGET_NAME}"
    VERBATIM
)
