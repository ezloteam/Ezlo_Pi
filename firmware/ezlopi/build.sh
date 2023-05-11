#!/bin/bash

project="Ezlo_Pi_v2x"
versionFile="components/version/include/version.h"
releaseNote="" # firmware/v$V_MAJOR_$V_MINOR_$V_BATCH_$V_BUILD/releaseNote.txt
release=0

if [[ "release" == $1 ]];then
release=1
elif [[ "test" == $1 ]];then
release=2
elif [[  "build" == $1 ]];then
release=3
elif [[ "compile_binary" == $1 ]];then
release=4
fi

V_MAJOR=2 # Major changes such as protocols, in-compatible APIs, Probably not compatible with prior version
V_MINOR=0 # Minor changes, Are always compatible with prior versions, eg. feature additions, 
V_BATCH=0 # Patch changes are like bug-fixes, security addition, and are strickly backward compatible
V_BUILD=0 # Build count, Incremental, Increases by 1 on each build call

S_MAJOR="MAJOR"
S_MINOR="MINOR"
S_BATCH="BATCH"
S_BUILD="BUILD"

get_version_variables() {
    if [ -e $versionFile ]; then
        while read line; 
        do
            if [ `echo $line | grep -c $S_BATCH ` -gt 0 ]; then
                read -a strarr <<< "$line"
                V_BATCH=${strarr[2]}
            elif [ `echo $line | grep -c $S_BUILD ` -gt 0 ]; then
                read -a strarr <<< "$line"
                V_BUILD=${strarr[2]}
            fi
        done < $versionFile
    else
        V_BUILD=$((V_BUILD+1))
        version_create
    fi
}

version_create () {
    echo -e "#ifndef __VERSION_H__" > $versionFile
    echo -e "#define __VERSION_H__" >> $versionFile
    echo -e "#ifdef __cplusplus" >> $versionFile
    echo -e "extern \"C\"" >> $versionFile
    echo -e "{" >> $versionFile
    echo -e "#endif" >> $versionFile
    if [ $release == 1 ];then # Releasing the firmware for deployment
        echo -e "#define V_TYPE 1" >> $versionFile
    else
        echo -e "#define V_TYPE 2" >> $versionFile
    fi
    echo -e "#define BUILD_DATE ${EPOCHSECONDS}" >> $versionFile
    echo -e "#define MAJOR ${V_MAJOR}" >> $versionFile
    echo -e "#define MINOR ${V_MINOR}" >> $versionFile
    echo -e "#define BATCH ${V_BATCH}" >> $versionFile
    echo -e "#define BUILD ${V_BUILD}" >> $versionFile
    if [ "add_build_version" == $1 ]; then
        echo -e "#define VERSION_STR \"${V_MAJOR}.${V_MINOR}.${V_BATCH}.${V_BUILD}\"" >> $versionFile
    else
        echo -e "#define VERSION_STR \"${V_MAJOR}.${V_MINOR}.${V_BATCH}\"" >> $versionFile
    fi
    echo -e "#ifdef __cplusplus" >> $versionFile
    echo -e "}" >> $versionFile
    echo -e "#endif" >> $versionFile
    echo -e "#endif // __VERSION_H__" >> $versionFile
}

version_update () {
    rm $versionFile
    version_create $1
}

wait_for_key() {
    while true; do
    read -p "Do you wish to create a release? [Y/y: Yes, N/n: No]: " yn
        case $yn in
            [Yy]* ) release=1; break;;
            [Nn]* ) release=0; break;;
            * ) echo "Please answer Y/y: Yes or N/n: no.";;
        esac
    done
}

copy_binaries() {
    cp $1/$project.bin $2/0x10000.bin
    cp $1/bootloader/bootloader.bin $2/0x1000.bin
    cp $1/partition_table/partition-table.bin $2/0x8000.bin
    cp $1/ota_data_initial.bin $2/0xd000.bin
}

create_release() {
    release_bins="firmware/v${V_MAJOR}_${V_MINOR}_${V_BATCH}"
    mkdir $release_bins
    copy_binaries build $release_bins
    # cp build/$project.bin $release_bins/10000.bin
    # cp build/bootloader/bootloader.bin $release_bins/
    # cp build/partition_table/partition-table.bin $release_bins/
    # cp build/ota_data_initial.bin $release_bins/
}

create_test_release() {
    release_bins="firmware/v${V_MAJOR}_${V_MINOR}_${V_BATCH}_${V_BUILD}"
    mkdir $release_bins
    copy_binaries build $release_bins
    # cp build/$project.bin $release_bins/0x10000.bin
    # cp build/bootloader/bootloader.bin $release_bins/0x1000.bin
    # cp build/partition_table/partition-table.bin $release_bins/0x8000.bin
    # cp build/ota_data_initial.bin $release_bins/0xd000.bin
}

release_note() {
    MAIN_NOTE=firmware/releaseNote.txt
    releaseFolder="v${V_MAJOR}_${V_MINOR}_${V_BATCH}"
    echo "Add release note:"
    read rel_note
    echo -e $releaseFolder >> $MAIN_NOTE
    var=$(date)
    echo -e "\tRelease Date: $var" >> $MAIN_NOTE
    echo -e "\t"${V_BUILD}": "$rel_note >> $MAIN_NOTE
    echo -e "\r\n\r\n" >> $MAIN_NOTE
    cp $MAIN_NOTE firmware/$releaseFolder/releaseNote.txt
}

test_release_note() {
    MAIN_NOTE=firmware/releaseNote.txt
    releaseFolder="v${V_MAJOR}_${V_MINOR}_${V_BATCH}_${V_BUILD}"
    echo "Add test release note:"
    read test_rel_note
    var=$(date)
    echo -e $releaseFolder " Date: "$var >> $MAIN_NOTE
    echo -e "\t"$test_rel_note"\r\n" >> $MAIN_NOTE
    cp $MAIN_NOTE firmware/$releaseFolder/releaseNote.txt
}

build_note() {
    MAIN_NOTE=firmware/releaseNote.txt
    releaseFolder="v${V_MAJOR}_${V_MINOR}_${V_BATCH}"
    echo "Add build note:"
    read bld_note
    var=$(date)
    echo -e "\t${V_BUILD} ($var): $bld_note" >> $MAIN_NOTE
}

idf.py build
retVal=$?
mkdir -p "firmware"
if [ $retVal -ne 1 ]; then
    get_version_variables
    V_BUILD=$((V_BUILD+1))

    if [[ 0 == release ]];then
        wait_for_key
    fi

    if [ $release == 1 ];then # Releasing the firmware for deployment
        V_BATCH=$((V_BATCH+1))
        version_update
        idf.py build
        create_release
        release_note
        release_bins="firmware/v${V_MAJOR}_${V_MINOR}_${V_BATCH}"
        zip -r $release_bins.zip $release_bins
        echo -e "Release "$release_bins".zip successfully created."
    elif [ 2 == $release ];then # creating test-release: Only for testing
        version_update add_build_version
        idf.py build
        create_test_release
        test_release_note
        release_bins="firmware/v${V_MAJOR}_${V_MINOR}_${V_BATCH}_${V_BUILD}"
        zip -r $release_bins.zip $release_bins
        echo -e "Test release "$release_bins".zip successfully created."
    elif [ 4 == $release ];then
        idf.py build
        create_release
        release_bins="firmware/v${V_MAJOR}_${V_MINOR}_${V_BATCH}_${V_BUILD}"
        zip -r $release_bins.zip $release_bins
        echo -e "Test release "$release_bins".zip successfully created."
    else # Build note only,
        version_update
        idf.py build
        build_note
    fi
fi

echo "MAJOR: $V_MAJOR"
echo "MINOR: $V_MINOR"
echo "BATCH: $V_BATCH"
echo "BUILD: $V_BUILD"
