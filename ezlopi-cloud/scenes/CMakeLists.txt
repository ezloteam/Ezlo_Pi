file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "scenes.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES frozen json trace
    ezlopi_factory_info
    ezlopi_cloud_constants 
    ezlopi_scenes)
