file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "room.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi_factory_info trace freertos ezlopi_websocket_client json ezlopi_cloud_constants ezlopi_devices
)