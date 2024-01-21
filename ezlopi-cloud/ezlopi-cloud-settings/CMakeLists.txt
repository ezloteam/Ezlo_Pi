file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES
    
    ezlopi-core-devices
    ezlopi-core-factory_info
    ezlopi-core-settings
    json    
    ezlopi-cloud-constants
    ezlopi-service-webprov
    trace) 