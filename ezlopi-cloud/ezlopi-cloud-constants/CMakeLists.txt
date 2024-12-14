file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES )