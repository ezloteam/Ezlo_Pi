file(GLOB_RECURSE dir_srcs "*.c*")
idf_component_register(SRCS "${dir_srcs}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-core-factory-info ezlopi-util-trace json ezlopi-core-devices)
