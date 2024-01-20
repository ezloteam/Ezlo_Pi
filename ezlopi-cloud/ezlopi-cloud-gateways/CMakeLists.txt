file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES trace json 
    ezlopi_factory_info
    ezlopi-cloud-constants 
    ezlopi_cloud 
    ezlopi_gateway)