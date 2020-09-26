OUTPUT_BASE_DIR ?= ..
LIBRARY_DIR ?= ../library

include $(LIBRARY_DIR)/ubinos/make/common.mk
-include $(LIBRARY_DIR)/ubinos/make/custom.mk

help config menuconfig xconfig build clean cleand doc all rebuild rebuildd load reset run debug attach env test:
	make -C $(CONFIG_DIR)/../make -f makefile.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) CONFIG_DIR=$(realpath $(CONFIG_DIR)) CONFIG_NAME=$(CONFIG_NAME) $@

zbatch-%:
	make -C ../library/ubinos/make                      -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/seggerrtt_wrapper/make           -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/nrf5sdk_wrapper/make             -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/stm32cubef2_wrapper/make         -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/esp8266at/make                   -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*


