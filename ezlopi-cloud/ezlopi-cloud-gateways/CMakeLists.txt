file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_gateways.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-util-trace json 
    ezlopi-core-factory-info
    ezlopi-cloud-constants 
    ezlopi-core-cloud 
    ezlopi-core-gateway)