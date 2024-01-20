file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi_factory_info trace freertos ezlopi_websocket_client web_provisioning version)