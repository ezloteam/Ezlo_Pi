file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES
    ezlopi_factory_info
    ezlopi_settings
    json
    ezlopi_devices
    ezlopi-cloud-constants
    web_provisioning
    trace) 