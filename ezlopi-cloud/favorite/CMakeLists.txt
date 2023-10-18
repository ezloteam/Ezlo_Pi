file(GLOB_RECURSE src_files "*.c")

idf_component_register(
    SRCS "favorite.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES        
        trace
        json
        ezlopi_cloud_constants
        ezlopi_devices
        ezlopi_factory_info
)
