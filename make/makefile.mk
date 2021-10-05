OUTPUT_BASE_DIR ?= ..
LIBRARY_DIR ?= ../library

include $(LIBRARY_DIR)/ubinos/make/common.mk
-include $(LIBRARY_DIR)/ubinos/make/custom.mk

help all config configd build clean cleand rebuild rebuildd debugs load run reset debug attach env cleanenv xconfig menuconfig doc test:
	make -C $(CONFIG_DIR)/../make -f makefile.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) CONFIG_DIR=$(realpath $(CONFIG_DIR)) CONFIG_NAME=$(CONFIG_NAME) $@

xsel:
	python3 "$(_UBINOS_DIR)/make/confsel.py" .. library

zbatch-%:
	make -C ../library/ubinos/make                      -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/CMSIS_5_wrapper/make             -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/seggerrtt_wrapper/make           -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/tflite-micro_wrapper/make        -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/nrf5sdk_wrapper/make             -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/nrf5sdk_extension/make           -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/stm32cubef2_wrapper/make         -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
	make -C ../library/stm32cubef2_extension/make       -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*
#	make -C ../library/esp8266at/make                   -f batch.mk OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $*

