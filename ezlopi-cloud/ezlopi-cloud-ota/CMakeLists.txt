file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-cloud-data
             trace
             json
             version
             ezlopi-core-factory_info
             ezlopi-core-devices
             ezlopi-core-event_group
             ezlopi-core-ota )
