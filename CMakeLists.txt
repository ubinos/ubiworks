cmake_minimum_required(VERSION 3.24)
project(ubiworks NONE)

include(config.cmake)

set(PROJECT_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(UBI_CONFIG "${UBI_CONFIG_DIR}/${UBI_CONFIG_NAME}")
set(UBI_BUILD_DIR_NAME "build")
set(UBI_BUILD_DIR "${PROJECT_BASE_DIR}/${UBI_BUILD_DIR_NAME}/${UBI_CONFIG_NAME}")

# set(_lib_dir "${PROJECT_BASE_DIR}/../ubiworks/lib")
set(_cmake_opt) # set(_cmake_opt -D PROJECT_LIB_DIR=${_lib_dir})
set(_ubitool_configsel_opt) # set(_ubitool_configsel_opt -l ${_lib_dir} -a)
set(_ubitool_libmgr_opt)

# CMake 3.12+면 자동 병렬
set(UBI_PARALLEL_FLAG "")
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
    set(UBI_PARALLEL_FLAG --parallel)
endif()

##
add_custom_target(all_build ALL
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} --build . --target rsync
    COMMAND ${CMAKE_COMMAND} --build . --target prebuild
    COMMAND ${CMAKE_COMMAND} --build . --target build
    COMMENT "---- All: rsync prebuild, build  (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

##
add_custom_target(xcm
    WORKING_DIRECTORY ${PROJECT_BASE_DIR}
    COMMAND ubitool configsel ${_ubitool_configsel_opt}
    VERBATIM
)

add_custom_target(xlm
    WORKING_DIRECTORY ${PROJECT_BASE_DIR}
    COMMAND ubitool libmgr ${_ubitool_libmgr_opt}
    VERBATIM
)

##
add_custom_target(configd
    COMMAND ${CMAKE_COMMAND} --build . --target cleand
    COMMAND ${CMAKE_COMMAND} --build . --target config
    COMMENT "---- Delete and make config (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(rebuildd
    COMMAND ${CMAKE_COMMAND} --build . --target configd
    COMMAND ${CMAKE_COMMAND} --build . --target rebuild
    COMMENT "---- Delete and make config, build (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

##
add_custom_target(config
    COMMAND ${CMAKE_COMMAND} -E make_directory "${UBI_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} -E chdir "${UBI_BUILD_DIR}"
            ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} -G "Unix Makefiles" "${UBI_CONFIG}" ${_cmake_opt}  --fresh
    COMMENT "---- Make config (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(cleand
    COMMAND ${CMAKE_COMMAND} --build . --target config
    COMMAND ${CMAKE_COMMAND} -E chdir "${UBI_BUILD_DIR}"
            ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target cleand --config $<CONFIG>
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${UBI_BUILD_DIR}"
    COMMENT "---- Delete config (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

##
add_custom_target(xconfig
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND cmake-gui "${UBI_CONFIG}"
    COMMENT "---- Config in GUI (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

##
add_custom_target(rsync
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target rsync --config $<CONFIG>
    COMMENT "---- Sync with remote (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(prebuild
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target prebuild --config $<CONFIG>
    COMMENT "---- Prebuild (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(build
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
    COMMENT "---- Rebuild (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(postbuild
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target postbuild --config $<CONFIG>
    COMMENT "---- Prebuild (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

##
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

add_custom_target(xrun
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target xrun --config $<CONFIG>
    COMMENT "---- Run with new window (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(debug
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target debug --config $<CONFIG>
    COMMENT "---- Debug (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(xdebug
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target xdebug --config $<CONFIG>
    COMMENT "---- Debug with new window (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(attach
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target attach --config $<CONFIG>
    COMMENT "---- Debug (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(xattach
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target xattach --config $<CONFIG>
    COMMENT "---- Debug with new window (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(dserver
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target dserver --config $<CONFIG>
    COMMENT "---- Debug server (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)

add_custom_target(xdserver
    WORKING_DIRECTORY ${UBI_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E env --unset=MAKEFLAGS
            ${CMAKE_COMMAND} --build . --target xdserver --config $<CONFIG>
    COMMENT "---- Debug Server with new window (UBI_CONFIG_NAME: ${UBI_CONFIG_NAME}, UBI_BUILD_DIR: ${UBI_BUILD_DIR})"
    VERBATIM
)
