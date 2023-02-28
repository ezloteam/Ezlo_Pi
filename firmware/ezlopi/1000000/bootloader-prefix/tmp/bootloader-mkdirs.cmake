# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v4.4.2/components/bootloader/subproject"
  "C:/Users/soc20/Ezlo_IR_Blaster/firmware/ezlopi/1000000/bootloader"
  "C:/Users/soc20/Ezlo_IR_Blaster/firmware/ezlopi/1000000/bootloader-prefix"
  "C:/Users/soc20/Ezlo_IR_Blaster/firmware/ezlopi/1000000/bootloader-prefix/tmp"
  "C:/Users/soc20/Ezlo_IR_Blaster/firmware/ezlopi/1000000/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/soc20/Ezlo_IR_Blaster/firmware/ezlopi/1000000/bootloader-prefix/src"
  "C:/Users/soc20/Ezlo_IR_Blaster/firmware/ezlopi/1000000/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/soc20/Ezlo_IR_Blaster/firmware/ezlopi/1000000/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
