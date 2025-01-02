file(GLOB_RECURSE dir_srcs "*.c*")
idf_component_register(SRCS "ezlopi_cloud_data.c" "${dir_srcs}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-core-factory-info ezlopi-util-trace cjext ezlopi-core-devices)
