file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "storagelib.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    )