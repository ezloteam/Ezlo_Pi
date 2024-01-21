file(GLOB_RECURSE dir_srcs "*.c*")
idf_component_register(SRCS "${dir_srcs}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-core-factory-info trace json ezlopi-core-devices)
