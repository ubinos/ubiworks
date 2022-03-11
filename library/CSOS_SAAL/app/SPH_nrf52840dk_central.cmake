# ubinos_config_info {"name_base": "SPH", "build_type": "cmake_ubinos", "app": true}

set_cache(UBINOS__UBIK__TICK_TYPE "RTC" STRING)
set_cache(UBINOS__UBIK__TICK_PER_SEC 1024 STRING)

set_cache(UBINOS__UBICLIB__EXCLUDE_CLI FALSE BOOL)

#set_cache(UBINOS__BSP__LINK_MEMMAP_RAM_ORIGIN 0x20003400 STRING)
#set_cache(UBINOS__BSP__LINK_MEMMAP_RAM_LENGTH 0x0003CC00 STRING)
set_cache(UBINOS__BSP__LINK_MEMMAP_RAM_ORIGIN 0x2000A000 STRING)
set_cache(UBINOS__BSP__LINK_MEMMAP_RAM_LENGTH 0x00036000 STRING)

set_cache(NRF5SDK__SWI_DISABLE0 TRUE BOOL)

set_cache(UBINOS__BSP__DTTY_TYPE "EXTERNAL" STRING)
set_cache(SEGGERRTT__DTTY_ENABLE TRUE BOOL)

set_cache(NRF5SDK__UART_ENABLED TRUE BOOL)
set_cache(NRF5SDK__NRFX_UARTE0_ENABLED TRUE BOOL)
set_cache(NRF5SDK__NRFX_UARTE1_ENABLED TRUE BOOL)
set_cache(NRF5SDK__TWI_ENABLED TRUE BOOL)

set_cache(UBINOS__UBIK__MSGQ_MSGSIZE_MAX 32 STRING)

set_cache(CSOS_SAAL__USE_LIB_433 TRUE BOOL)
set_cache(CSOS_SAAL__USE_LIB_twi_internal_sensors TRUE BOOL)
set_cache(CSOS_SAAL__USE_LIB_wifi_wizfi360 TRUE BOOL)

include(${PROJECT_UBINOS_DIR}/config/ubinos_nrf52840dk_softdevice.cmake)
include(${PROJECT_LIBRARY_DIR}/seggerrtt_wrapper/config/seggerrtt.cmake)
include(${PROJECT_LIBRARY_DIR}/nrf5sdk_wrapper/config/nrf5sdk.cmake)
include(${PROJECT_LIBRARY_DIR}/nrf5sdk_extension/config/nrf5sdk_extension.cmake)

include(${PROJECT_LIBRARY_DIR}/CSOS_SAAL/config/csos_saal.cmake)

set(_tmp_all_flags "-Wno-error=unused-variable -Wno-error=unused-function -Wno-error=unused-const-variable -Wno-error=unused-but-set-variable -Wno-error=comment -Wno-error=int-conversion -Wno-error=implicit-function-declaration -Wno-error=incompatible-pointer-types -Wno-error=discarded-qualifiers -Wno-error=pointer-sign")

set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${_tmp_all_flags}")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${_tmp_all_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_tmp_all_flags}")

####

set(INCLUDE__APP TRUE)
set(APP__NAME "SPH")

get_filename_component(_tmp_source_dir "${CMAKE_CURRENT_LIST_DIR}/${APP__NAME}" ABSOLUTE)
string(TOLOWER ${UBINOS__BSP__BOARD_MODEL} _temp_board_model)
string(TOLOWER ${UBINOS__BSP__NRF52_SOFTDEVICE_NAME} _temp_softdevice_name)

include_directories(${_tmp_source_dir}/arch/arm/cortexm/${_temp_board_model}/${_temp_softdevice_name}/config)
include_directories(${_tmp_source_dir}/arch/arm/cortexm/${_temp_board_model})
include_directories(${_tmp_source_dir})

include_directories(${_tmp_source_dir}/config/central)

file(GLOB_RECURSE _tmp_sources
    "${_tmp_source_dir}/*.c"
    "${_tmp_source_dir}/*.cpp"
    "${_tmp_source_dir}/*.S"
    "${_tmp_source_dir}/*.s")

set(PROJECT_APP_SOURCES ${PROJECT_APP_SOURCES} ${_tmp_sources})

