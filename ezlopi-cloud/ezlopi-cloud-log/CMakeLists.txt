file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_log.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES  
                cjext 
                ezlopi-core-log
                ezlopi-core-sntp
                ezlopi-cloud-constants)