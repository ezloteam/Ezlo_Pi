file(GLOB_RECURSE src_files "*.c")

idf_component_register(
    SRCS "devices.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES
        json
        ezlopi_factory_info
        trace
        ezlopi_devices
        ezlopi_cloud_constants
)
