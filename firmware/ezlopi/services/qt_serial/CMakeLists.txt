# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "./*.c*")

idf_component_register(SRCS "qt_serial.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos interface_common devices_common interface_common
    nvs_flash wifi_interface debug json nvs_storage factory_info version)