file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "room.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES json trace freertos ezlopi_cloud_constants ezlopi_room ezlopi_nvs )
    