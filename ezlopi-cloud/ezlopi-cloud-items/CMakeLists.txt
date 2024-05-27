file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "ezlopi_cloud_items.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-core-factory-info ezlopi-util-trace cjext ezlopi-cloud-constants ezlopi-core-devices ezlopi-service-webprov)
