file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_info.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES  ezlopi-core-factory-info ezlopi-util-trace  ezlopi-util-version json ezlopi-cloud-constants)