file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "ezlopi_cloud_registration.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-core-factory-info ezlopi-util-trace freertos ezlopi-core-websocket_client ezlopi-service-webprov ezlopi-util-version)