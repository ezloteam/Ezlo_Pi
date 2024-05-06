speed=921600
port="/dev/ttyUSB0"

len=0x8000
address=0x18000

while getopts s:p:l:a:h: flag
do
    case "${flag}" in
        s)  speed=${OPTARG};;
        p)  port=${OPTARG};;
        l)  len=${OPTARG};;
        a)  address=${OPTARG};;
        h)  echo "run ${0} <option>";
            echo "options: [-s] [-p] [-l] [-a] [-h]"; echo "";
            echo "[-s]      serila baud rate, default value is 921600";
            echo "[-p]      serial port, default is /dev/ttyUSB0";
            echo "[-l]      lenght of binary data to be read";
            echo "[-a]      binary file which contains provisioning data";
            echo "[-h]      help";
            exit 0

    esac
done

echo ""; echo "";
echo "speed:    $speed";
echo "port:     $port";
echo "len:      $len";
echo "address:  $address";
echo ""; echo "";

esptool.py -p $port -b $speed read_flash $address $len ./shell_script/prov_info.bin

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