file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS "devices.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES json frozen ezlopi_factory_info trace devices_common ezlopi_devices
)