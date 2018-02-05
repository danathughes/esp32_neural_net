deps_config := \
	/home/dana/esp/esp-idf/components/app_trace/Kconfig \
	/home/dana/esp/esp-idf/components/aws_iot/Kconfig \
	/home/dana/esp/esp-idf/components/bt/Kconfig \
	/home/dana/esp/esp-idf/components/esp32/Kconfig \
	/home/dana/esp/esp-idf/components/ethernet/Kconfig \
	/home/dana/esp/esp-idf/components/fatfs/Kconfig \
	/home/dana/esp/esp-idf/components/freertos/Kconfig \
	/home/dana/esp/esp-idf/components/heap/Kconfig \
	/home/dana/esp/esp-idf/components/libsodium/Kconfig \
	/home/dana/esp/esp-idf/components/log/Kconfig \
	/home/dana/esp/esp-idf/components/lwip/Kconfig \
	/home/dana/esp/esp-idf/components/mbedtls/Kconfig \
	/home/dana/esp/esp-idf/components/openssl/Kconfig \
	/home/dana/esp/esp-idf/components/pthread/Kconfig \
	/home/dana/esp/esp-idf/components/spi_flash/Kconfig \
	/home/dana/esp/esp-idf/components/spiffs/Kconfig \
	/home/dana/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/dana/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/dana/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/dana/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/dana/Research/smartTire/code/esp_code/data_collect/main/Kconfig.projbuild \
	/home/dana/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/dana/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
