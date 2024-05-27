file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_location.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES  
                cjext 
                micro_tz_db
                ezlopi-util-trace
                ezlopi-core-devices
                ezlopi-core-sntp
                ezlopi-cloud-constants)