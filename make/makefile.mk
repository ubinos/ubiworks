LIBRARY_DIR ?= ../library
OUTPUT_BASE_DIR ?= ..

LIBRARY_UPGRADE_LIST ?= ubinos seggerrtt seggerrtt_wrapper CMSIS_5 CMSIS_5_wrapper nrf5sdk_wrapper nrf5sdk_extension nrf5sdk_v17.00.00_lite stm32cubef2_wrapper stm32cubef2_extension stm32cubef2_v01.09.00_lite esp8266at tflite-micro_wrapper tflite-micro ArduinoCore-API_wrapper ArduinoCore-API Adafruit_BusIO_wrapper Adafruit_BusIO Adafruit_Sensor_wrapper Adafruit_Sensor Arduino_LSM9DS1_wrapper Arduino_LSM9DS1 Adafruit_BME280_Library_wrapper Adafruit_BME280_Library DHT-sensor-library_wrapper DHT-sensor-library libcrc libcrc_wrapper jansson jansson_wrapper mp_course_examples kissfft flatbuffers gemmlowp ruy

help: common-help

include $(LIBRARY_DIR)/ubinos/make/common.mk
-include $(LIBRARY_DIR)/ubinos/make/custom.mk

%: common-% ;

zbatch-%: common-zbatch-%
	make -C ../library/ubinos/make                              OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/CMSIS_5_wrapper/make                     OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/seggerrtt_wrapper/make                   OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/nrf5sdk_wrapper/make                     OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/stm32cubef2_wrapper/make                 OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/tflite-micro_wrapper/make                OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/ArduinoCore-API_wrapper/make             OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/Adafruit_BusIO_wrapper/make              OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/Adafruit_BME280_Library_wrapper/make     OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/Arduino_LSM9DS1_wrapper/make             OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/DHT-sensor-library_wrapper/make          OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/libcrc_wrapper/make                      OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/jansson_wrapper/make                     OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	make -C ../library/mp_course_examples/make                  OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@
	# make -C ../library/esp8266at/make                           OUTPUT_BASE_DIR=$(realpath $(OUTPUT_BASE_DIR)) LIBRARY_DIR=$(realpath $(LIBRARY_DIR)) $@

