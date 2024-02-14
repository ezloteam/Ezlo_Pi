file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_ota.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-cloud-data
             ezlopi-util-trace
             json
             ezlopi-util-version
             ezlopi-core-factory-info
             ezlopi-core-devices
             ezlopi-core-event-group
             ezlopi-core-ota )
