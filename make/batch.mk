###############################################################################

%:
	@echo "==============================================================================="
	@echo ""

	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_sam9xe512ek_baremetal_sram
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_sam9xe512ek_baremetal
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_sam9xe512ek
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/app CONFIG_NAME=helloworld_sam9xe512ek_baremetal_sram
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/app CONFIG_NAME=helloworld_sam9xe512ek_baremetal
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/app CONFIG_NAME=helloworld_mt_sam9xe512ek
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/app CONFIG_NAME=ubik_tester_sam9xe512ek
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""

	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk_baremetal_sram
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk_baremetal
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk_baremetal_trace
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk_trace
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk_softdevice_baremetal
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk_softdevice_baremetal_trace
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk_softdevice
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/config CONFIG_NAME=ubinos_nrf52dk_softdevice_trace
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/app CONFIG_NAME=helloworld_nrf52dk_baremetal_sram
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/app CONFIG_NAME=helloworld_nrf52dk_baremetal
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/app CONFIG_NAME=helloworld_mt_nrf52dk
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/ubinos/app CONFIG_NAME=ubik_tester_nrf52dk
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""

	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/nrf5sdk/config CONFIG_NAME=nrf5sdk_nrf52dk_baremetal
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""

	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/nrf5example/app CONFIG_NAME=myrtc2_nrf52dk
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/nrf5example/app CONFIG_NAME=myble_app_blinky2_nrf52dk
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/nrf5example/app CONFIG_NAME=myble_app_blinky2_c_nrf52dk
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""
	make -f makefile.mk $@ CONFIG_DIR=../library/nrf5example/app CONFIG_NAME=myble_app_multilink_central2_nrf52dk
	@echo ""
	@echo "-------------------------------------------------------------------------------"
	@echo ""

	@echo ""
	@echo "==============================================================================="

###############################################################################


