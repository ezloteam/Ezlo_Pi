file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_core_log.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES  
                ezlopi-core-sntp
                ezlopi-core-ezlopi
                ezlopi-core-ezlopi
                ezlopi-cloud-constants
                ezlopi-core-errors
                )



