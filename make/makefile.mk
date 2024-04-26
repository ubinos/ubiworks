LIBRARY_DIR ?= ../library
OUTPUT_BASE_DIR ?= ..

help: common-help

include $(LIBRARY_DIR)/ubinos/make/common.mk
-include $(LIBRARY_DIR)/ubinos/make/custom.mk

%: common-% ;

ifeq ("$(wildcard $(CONFIG_DIR)/$(CONFIG_NAME).cmake)","") # if $(CONFIG_DIR)/$(CONFIG_NAME).cmake is not exist then
ifneq ("$(wildcard $(CONFIG_DIR)/$(CONFIG_NAME).mk)","")# if $(CONFIG_DIR)/$(CONFIG_NAME).mk is exist then
BUILD_TYPE_MAKE__MAKE_DIR = $(shell python "$(_TOOLBOX)" get_make_dir_from_config_file $(CONFIG_DIR)/$(CONFIG_NAME).mk)
all config configd build clean cleand rebuild rebuildd dserver xdserver load reset run xrun debug xdebug attach xattach xconfig menuconfig doc cleandoc xopendoc env cleanenv test:
ifneq ($(strip $(BUILD_TYPE_MAKE__MAKE_DIR)),)
	make -C ../$(BUILD_TYPE_MAKE__MAKE_DIR) -f makefile.mk          OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) CONFIG_DIR=$(realpath $(CONFIG_DIR)) CONFIG_NAME=$(CONFIG_NAME) DEBUG_SERVER_SERIAL=$(DEBUG_SERVER_SERIAL) DEBUG_SERVER_PORT=$(DEBUG_SERVER_PORT) $@
endif
endif
endif

zbatch-%: common-zbatch-%
	make -C ../library/ubinos/make                                  OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
