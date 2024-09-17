file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "ezlopi_cloud_registration.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES            freertos 
                        ezlopi-core-factory-info 
                        ezlopi-util-trace 
                        ezlopi-core-websocket_client 
                        ezlopi-core-processes 
                        ezlopi-core-errors
                        ezlopi-service-webprov 
                        ezlopi-util-version
                        ezlopi-service-loop
    )