file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS  "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi_factory_info trace ezlopi_factory_info trace json ezlopi_cloud_constants ezlopi_scenes)