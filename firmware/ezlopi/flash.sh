#!/bin/bash
# controller="esp32s3"
controller="esp32"
usb_port="/dev/ttyUSB0"
baud_rate=921600

firmware_folder="/home/krishna/Desktop/v2_0_6_134/firmware/esp32_4mb"
# firmware_folder="/home/krishna/projects/nds/ezlo/Ezlo_Pi/firmware/ezlopi/firmware/v2_0_6_133"


bootloader="0x1000.bin"
partition_table="0x8000.bin"
ota_data_initial="0xd000.bin"
firmware="0x10000.bin"

# bootloader_loc="0x0000" # esp32s3
bootloader_loc="0x1000" # esp32
partition_table_loc="0x8000"
ota_data_initial_loc="0xd000"
firmware_loc="0x10000"

# . ~/esp/esp-idf/export.sh
esptool.py -p $usb_port -b $baud_rate --before default_reset --after hard_reset --chip $controller  write_flash --flash_mode dio --flash_size detect --flash_freq 40m $bootloader_loc $firmware_folder/$bootloader $partition_table_loc $firmware_folder/$partition_table $ota_data_initial_loc $firmware_folder/$ota_data_initial $firmware_loc $firmware_folder/$firmware


# esptool.py -p (PORT) -b 460800 --before default_reset --after hard_reset --chip esp32  write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin 0xd000 build/ota_data_initial.bin 0x10000 build/Ezlo_Pi_v2x.bin


