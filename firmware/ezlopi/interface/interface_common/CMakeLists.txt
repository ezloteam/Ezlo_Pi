# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "src/*.c*")

idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "include"
    REQUIRES driver freertos nvs_storage debug)