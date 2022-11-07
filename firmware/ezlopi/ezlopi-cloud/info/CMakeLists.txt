file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "info.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES frozen ezlopi_factory_info trace  version json ezlopi_cloud_constants)