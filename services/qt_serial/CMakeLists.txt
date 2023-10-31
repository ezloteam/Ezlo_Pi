# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "./*.c*")

idf_component_register(SRCS "qt_serial.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos nvs_flash ezlopi_wifi trace json ezlopi_nvs ezlopi_factory_info version log ezlopi_hal)