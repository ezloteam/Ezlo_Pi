#!/bin/bash
controller="esp32s3"
# controller="esp32"
usb_port="/dev/ttyACM0"
# usb_port="/dev/ttyUSB0"
baud_rate=921600
fimrware_loc="firmware/id"
id_bin="100002119.bin"
# id_bin="100005802.bin"


esptool.py -p $usb_port -b $baud_rate --before default_reset --after hard_reset --chip $controller  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x3b0000 $fimrware_loc/$id_bin
