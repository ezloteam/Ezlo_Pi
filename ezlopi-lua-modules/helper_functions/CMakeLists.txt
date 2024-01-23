file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "lua_helper_functions.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES )