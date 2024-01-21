# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "./*.c*")

idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos nvs_flash ezlopi-core-wifi trace json ezlopi-core-nvs ezlopi-core-factory_info version log  ezlopi-cloud-info ezlopi-core-net ezlopi-core-reboot)