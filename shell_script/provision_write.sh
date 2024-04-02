. ~/esp/esp-idf/export.sh
esptool.py -p /dev/ttyUSB0 -b 921600 write_flash 0x18000 ./shell_script/prov_102002011.bin