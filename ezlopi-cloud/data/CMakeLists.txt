# Find all source files recursively
file(GLOB_RECURSE src_files "*.c*")

# Register the component with IDF
idf_component_register(
    SRCS "data.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES trace
             json
             ezlopi_factory_info
             ezlopi_devices
             ezlopi_cloud_constants
    )
