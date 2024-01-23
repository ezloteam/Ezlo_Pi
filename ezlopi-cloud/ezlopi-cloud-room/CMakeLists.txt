file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES json ezlopi-util-trace freertos ezlopi-cloud-constants ezlopi-core-nvs ezlopi-core-devices ezlopi-core-room )
    