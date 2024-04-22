file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "ezlopi_cloud_settings.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    cjext  
                ezlopi-util-trace
                ezlopi-core-devices
                ezlopi-core-factory-info
                ezlopi-core-settings  
                ezlopi-cloud-constants
                ezlopi-service-webprov
) 