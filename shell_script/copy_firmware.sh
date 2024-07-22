#!/bin/bash

build=188
version="3.5.8"
chip="ESP32S3"

app="Ezlo_Pi_v3x"
distro="full_option"

bootloader_address=0x0
firmware_address="0x20000"
partition_table_address="0x8000"
ota_data_address="0x15000"

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
echo "firmware-destination: ${path}"
echo ""
mkdir -p ${path}

cp -p ./build/${app}.bin ${path}
cp -p ./build/bootloader/bootloader.bin ${path}
cp -p ./build/partition_table/partition-table.bin ${path}
cp -p ./build/ota_data_initial.bin ${path}


if [ "ESP32" = "${chip}" ]; then
bootloader_address=0x1000
elif [ "ESP32S3" = "${chip}" ]; then
echo ""
elif [ "ESP32C3" = "${chip}" ]; then
echo ""
else
echo ""
exit 0
fi


json_info="{\"version\": \"${version}\",\"build\": $build,\"build_date\": \"$(date)\",\"chip_type\": \"${chip}\",\"flash_size\": \"4MB\",\"binaries\": [{\"address\": \"${bootloader_address}\",\"file\": \"bootloader.bin\"},{\"address\": \"${firmware_address}\",\"file\": \"${app}.bin\"},{\"address\": \"${partition_table_address}\",\"file\": \"partition-table.bin\"},{\"address\": \"${ota_data_address}\",\"file\": \"ota_data_initial.bin\"}]}"
echo "json-info:${json_info}"
echo ${json_info} > ${path}/info.json

