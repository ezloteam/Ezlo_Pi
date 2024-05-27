speed=921600
chip="esp32s3"
address="0x18000"
port="/dev/ttyUSB0"
bin="./shell_script/prov_102002011.bin"

while getopts s:p:b:h:a:c: flag
do
    case "${flag}" in
        s)  speed=${OPTARG};;
        p)  port=${OPTARG};;
        b)  bin=${OPTARG};;
        c)  chip=${OPTARG};;
        a)  address=${OPTARG};;
        h)  echo "run ${0} <option>";
            echo "options: [-s] [-p] [-b] [-c]\r\n";
            echo "[-p]      serial port, default is /dev/ttyUSB0";
            echo "[-s]      serila baud rate, default value is 921600";
            echo "[-b]      binary file which contains provisioning data";
            echo "[-c]      chip type (eg. esp32, esp32s3, esp32c3, etc.)";
            echo "[-a]      address for provisioning info in hex (default address is 0x18000)";
            exit 0
    esac
done


echo "bin:   $bin";
echo "chip:  $chip";
echo "port:  $port";
echo "speed: $speed";
echo "address: $address";

echo "esptool.py -p $port -b $speed --chip ${chip} write_flash ${address} $bin"
esptool.py -p $port -b $speed --chip ${chip} write_flash ${address} $bin