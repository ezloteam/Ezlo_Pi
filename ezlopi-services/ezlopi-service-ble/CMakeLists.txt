# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c*")

idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES driver freertos trace ezlopi-core-devices ezlopi-core-ble ezlopi-core-factory_info ezlopi-core-nvs ezlopi-core-ping ezlopi-core-reboot) 
