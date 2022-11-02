file(GLOB_RECURSE src_files "src/*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "include"
    REQUIRES frozen
    ezlopi_factory_info
    trace
    freertos
    websocket_client
    json
    ezlopi_cloud_constants
    ezlopi_devices
)