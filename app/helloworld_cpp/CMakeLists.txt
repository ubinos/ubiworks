cmake_minimum_required(VERSION 3.24)

set(PROJECT_TARGET_NAME "app")

set(PROJECT_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
set(PROJECT_LIB_DIR "${PROJECT_BASE_DIR}/lib")
set(PROJECT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")

##
include("${PROJECT_BASE_DIR}/common/common_config.cmake")
include("${PROJECT_BASE_DIR}/common/common_c_cpp_local_pre.cmake")


project(${PROJECT_TARGET_NAME} C CXX ASM)

##
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SRC_DIR}/*.c"
    "${PROJECT_SRC_DIR}/*.cpp"
)

add_executable(${PROJECT_TARGET_NAME}
    ${APP_SOURCES}
)

target_include_directories(${PROJECT_TARGET_NAME} PRIVATE
    "${PROJECT_SRC_DIR}"
)

##
include("${PROJECT_BASE_DIR}/common/common_c_cpp_local_post.cmake")
