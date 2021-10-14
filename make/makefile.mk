LIBRARY_DIR ?= ../library
OUTPUT_BASE_DIR ?= ..

help: common-help

include $(LIBRARY_DIR)/ubinos/make/common.mk
-include $(LIBRARY_DIR)/ubinos/make/custom.mk

%: common-% ;

zbatch-%: common-zbatch-%
	make -C ../library/ubinos/make            	                OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/CMSIS_5_wrapper/make                     OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/seggerrtt_wrapper/make                   OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/nrf5sdk_wrapper/make                     OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/stm32cubef2_wrapper/make                 OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/tflite-micro_wrapper/make                OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	# make -C ../library/esp8266at/make                           OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@

