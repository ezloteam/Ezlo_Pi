#!/bin/bash
controller="esp32s3"
usb_port="/dev/ttyUSB1"
baud_rate=921600
fimrware_loc="/home/krishna/Desktop/v2_0_5"
bootloader="0x1000.bin"
partition_table="0x8000.bin"
ota_data_initial="0xd000.bin"
firmware="0x10000.bin"

esptool.py -p $usb_port -b $baud_rate --before default_reset --after hard_reset --chip $controller  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x0 $fimrware_loc/$bootloader 0x8000 $fimrware_loc/$partition_table 0xd000 $fimrware_loc//$ota_data_initial 0x10000 $fimrware_loc/$firmware





