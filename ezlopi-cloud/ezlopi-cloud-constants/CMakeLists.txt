file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_methods_str.c" "ezlopi_cloud_keywords.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES )