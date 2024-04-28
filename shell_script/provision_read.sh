. ~/esp/esp-idf/export.sh
esptool.py -p /dev/ttyACM0 -b 921600 read_flash 0x18000 0x8000 ./shell_script/prov_info.bin

a=$((0x$(xxd -ps -l 1 -s 0x6008 ./shell_script/prov_info.bin)))
b=$((0x$(xxd -ps -l 1 -s 0x6009 ./shell_script/prov_info.bin)))
c=$((0x$(xxd -ps -l 1 -s 0x600A ./shell_script/prov_info.bin)))
d=$((0x$(xxd -ps -l 1 -s 0x600B ./shell_script/prov_info.bin)))
e=$((0x$(xxd -ps -l 1 -s 0x600C ./shell_script/prov_info.bin)))
f=$((0x$(xxd -ps -l 1 -s 0x600D ./shell_script/prov_info.bin)))
g=$((0x$(xxd -ps -l 1 -s 0x600E ./shell_script/prov_info.bin)))
h=$((0x$(xxd -ps -l 1 -s 0x600F ./shell_script/prov_info.bin)))

serial_number=$(($a+($b<<8)+($c<<16)+($d<<24)+($e<<32)+($f<<40)+($g<<48)+($h<<56)))
mv ./shell_script/prov_info.bin ./shell_script/prov_$serial_number.bin