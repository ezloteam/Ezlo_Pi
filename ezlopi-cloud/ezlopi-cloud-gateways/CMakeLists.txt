file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES trace json 
    ezlopi-core-factory-info
    ezlopi-cloud-constants 
    ezlopi-core-cloud 
    ezlopi-core-gateway)