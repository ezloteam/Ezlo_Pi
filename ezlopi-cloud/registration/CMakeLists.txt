file(GLOB_RECURSE src_files "./*.c*")
idf_component_register(SRCS "registration.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES version ezlopi_factory_info trace freertos ezlopi_websocket_client web_provisioning)