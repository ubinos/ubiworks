cmake_minimum_required(VERSION 3.24)
project(ubiworks NONE)

include(config.cmake)

set(PROJECT_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(UBI_CONFIG "${UBI_CONFIG_DIR}/${UBI_CONFIG_NAME}")
set(UBI_BUILD_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/build")
set(UBI_BUILD_DIR "${UBI_BUILD_BASE_DIR}/${UBI_CONFIG_NAME}")

# CMake 3.12+면 자동 병렬
set(UBI_PARALLEL_FLAG "")
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
    set(UBI_PARALLEL_FLAG --parallel)
endif()

add_custom_target(_delete_build_base
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${UBI_BUILD_BASE_DIR}"
    COMMENT "---- Delete build directory (UBI_CONFIG: ${UBI_CONFIG}, UBI_BUILD_BASE_DIR: ${UBI_BUILD_BASE_DIR})"
    VERBATIM
)


add_custom_target(_dserver
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target dserver ${UBI_PARALLEL_FLAG} --config $<CONFIG>
    COMMENT "---- Debug server (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)


add_custom_target(cleand
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${UBI_BUILD_DIR}"
    COMMENT "---- Clean, delete (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(config
    COMMAND ${CMAKE_COMMAND} -E make_directory "${UBI_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E chdir "${UBI_BUILD_DIR}"
            ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} -G "Unix Makefiles" "${UBI_CONFIG}" --fresh
    COMMENT "---- Config (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(configd
    COMMAND ${CMAKE_COMMAND} --build . --target cleand
    COMMAND ${CMAKE_COMMAND} --build . --target config
    COMMENT "---- Clean, delete, config (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(xconfig
    COMMAND ${CMAKE_COMMAND} -E chdir "${UBI_BUILD_DIR}"
            cmake-gui "${UBI_CONFIG}"
    COMMENT "---- Config in GUI (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(build ALL
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target build ${UBI_PARALLEL_FLAG} --config $<CONFIG>
    COMMENT "---- Build (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(rebuild
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target rebuild ${UBI_PARALLEL_FLAG} --config $<CONFIG>
    COMMENT "---- Clean, build (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(rebuildd
    COMMAND ${CMAKE_COMMAND} --build . --target cleand
    COMMAND ${CMAKE_COMMAND} --build . --target config
    COMMAND ${CMAKE_COMMAND} --build . --target build
    COMMENT "---- Clean, delete, config, build (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(reset
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target reset --config $<CONFIG>
    COMMENT "---- Load (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(load
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target load --config $<CONFIG>
    COMMENT "---- Load (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(run
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target run --config $<CONFIG>
    COMMENT "---- Run (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(xdserver
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target xdserver --config $<CONFIG>
    COMMENT "---- Debug Server with new window (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(xlibmgr
    WORKING_DIRECTORY ${PROJECT_BASE_DIR}
    COMMAND ubitool libmgr
    VERBATIM
)

add_custom_target(xconfigsel
    WORKING_DIRECTORY ${PROJECT_BASE_DIR}
    COMMAND ubitool configsel
    VERBATIM
)
