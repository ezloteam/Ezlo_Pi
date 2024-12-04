#!/bin/bash

$get_idf
# esptool.py -p /dev/ttyUSB1 read_flash 0x18000 0x8000 105200657.bin

helpFunction()
{
   echo ""
   echo "Usage: $0 -p /dev/ttyUSBx -a 0x18000 -s 0x8000 -f 105200657.bin"
   echo -e "\t-p \t usb-port, eg. for most cases it is '/dev/ttyUSB0'"
   echo -e "\t-a \t address of flash from where the reading should start"
   echo -e "\t-s \t size/amount of data needs to be read from flash"
   echo -e "\t-f \t file name to store read binary data, Normally controller serial number (i.e. 105200657.bin)"
   exit 1 # Exit script after printing help
}

while getopts "p:a:s:f:" opt
do
   case "$opt" in
      p ) port="$OPTARG" ;;
      a ) address="$OPTARG" ;;
      s ) size="$OPTARG" ;;
      f ) file="$OPTARG" ;;
      h ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$port" ] || [ -z "$address" ] || [ -z "$size" ] || [ -z "$file" ]
then
   echo "Some or all of the parameters are empty";
   helpFunction
fi

# Begin script in case all parameters are correct
esptool.py -p "$port" read_flash $address $size $file