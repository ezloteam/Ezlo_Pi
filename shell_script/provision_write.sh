. ~/esp/esp-idf/export.sh


speed=921600
port="/dev/ttyUSB0"
bin="prov_102002011.bin"

while getopts s:p:b:h: flag
do
    case "${flag}" in
        s)  speed=${OPTARG};;
        p)  port=${OPTARG};;
        b)  bin=${OPTARG};;
        h)  echo "run ${0} <option>";
            echo "options: [-s] [-p] [-b]\r\n";
            echo "[-s]      serila baud rate, default value is 921600";
            echo "[-p]      serial port, default is /dev/ttyUSB0";
            echo "[-b]      binary file which contains provisioning data";

    esac
done


echo "speed: $speed";
echo "port:  $port";
echo "bin:   $bin";

echo "esptool.py -p $port -b $speed write_flash 0x18000 ./shell_script/$bin"
esptool.py -p $port -b $speed write_flash 0x18000 ./shell_script/$bin