# esptool.py -p /dev/ttyUSB0 -b 921600 erase_flash
usb_port=/dev/ttyUSB0
baud_rate=921600
esptool.py -p $usb_port -b $baud_rate erase_flash