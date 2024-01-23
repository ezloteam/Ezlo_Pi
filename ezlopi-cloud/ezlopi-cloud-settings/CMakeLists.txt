file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES
    
    ezlopi-util-trace
    json  
    ezlopi-core-devices
    ezlopi-core-factory-info
    ezlopi-core-settings  
    ezlopi-cloud-constants
    ezlopi-service-webprov
    ) 