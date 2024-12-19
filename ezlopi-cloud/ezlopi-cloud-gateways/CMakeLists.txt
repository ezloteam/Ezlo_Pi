file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "gateways_commands_str.c" "ezlopi_cloud_gateways.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-util-trace cjext 
    ezlopi-core-factory-info
    ezlopi-cloud-constants 
    ezlopi-core-cloud 
    ezlopi-core-gateway)