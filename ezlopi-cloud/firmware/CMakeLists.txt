file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "firmware.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES frozen ezlopi_factory_info trace json ezlopi_devices ezlopi_ota version)