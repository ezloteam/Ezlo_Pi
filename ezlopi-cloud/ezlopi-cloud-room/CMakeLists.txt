file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_room.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES cjext ezlopi-util-trace freertos ezlopi-cloud-constants ezlopi-core-nvs ezlopi-core-devices ezlopi-core-room )
    