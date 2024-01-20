file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-cloud-data
             trace
             json
             version
             ezlopi_factory_info
             ezlopi_devices
             ezlopi_event_group
             ezlopi_ota )
