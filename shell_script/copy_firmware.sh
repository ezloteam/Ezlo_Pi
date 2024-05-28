#!/bin/bash

build=188
version="3.5.8"
chip="ESP32S3"

app="Ezlo_Pi_v3x"
distro="full_option"

echo "current arguments value:"
echo "chip: ${chip}"
echo "version: ${version}"
echo "build: ${build}"
echo "app: ${app}"
echo "distro: ${distro}"
echo ""

while getopts v:b:c:a:d:h flag
do
    case "${flag}" in
        v)  version=${OPTARG};;
        b)  build=${OPTARG};;
        c)  chip=${OPTARG};;
        a)  app=${OPTARG};;
        d)  distro=${OPTARG};;

        h)  echo "run ${0} <option>";
            echo "options: [-v] [-b] [-c] [-a] [-d]"; echo "";
            echo "[-v]      firmware-version, eg: 3.5.8";
            echo "[-b]      build-number, eg: 188";
            echo "[-c]      chip-type, eg: ESP32, ESP32S3, ESP32C3";
            echo "[-a]      app-name, eg: Ezlo_Pi_v3x";
            echo "[-d]      distro type, options: full_option, cloud_meshbot, minimal";
            echo "[-h]      help";
            exit 0
    esac
done

path="./firmware/v${version}.${build}/${chip}/${distro}"
mkdir -p ${path}

cp -p ./build/${app}.bin ${path}
cp -p ./build/bootloader/bootloader.bin ${path}
cp -p ./build/partition_table/partition-table.bin ${path}
cp -p ./build/ota_data_initial.bin ${path}


bootloader_address=0x0
if ["ESP32" -eq "${chip}"]; then
bootloader_address=0x1000
elif ["ESP32S3" -eq "${chip}"]; then
elif ["ESP32C3" -eq "${chip}"]; then
else
echo "
exit 0
fi


json_info="{\"version\": \"${version}\",\"build\": $build,\"build_date\": \"$(date)\",\"chip_type\": \"${chip}\",\"flash_size\": [\"4MB\", \"8MB\"],\"binaries\": [{\"address\": \"0x0\",\"file\": \"bootloader.bin\"},{\"address\": \"0x30000\",\"file\": \"Ezlo_Pi_v3x.bin\"},{\"address\": \"0xF000\",\"file\": \"partition-table.bin\"},{\"address\": \"0x1C000\",\"file\": \"ota_data_initial.bin\"}]}"
echo ${json_info} > ${path}/info.json

