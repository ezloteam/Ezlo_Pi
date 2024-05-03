file(GLOB_RECURSE src_files "*.c")
idf_component_register(SRCS  "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES cjext ezlopi-core-factory-info ezlopi-util-trace  ezlopi-core-devices ezlopi-cloud-constants
)
