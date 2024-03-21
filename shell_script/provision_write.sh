. ~/esp/esp-idf/export.sh
esptool.py -p /dev/ttyACM0 -b 921600 write_flash 0x18000 ./shell_script/prov_102001583.bin