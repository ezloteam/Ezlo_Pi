file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "gateways_commands_str.c" "gateways.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES trace json 
    ezlopi_factory_info
    ezlopi_cloud_constants 
    ezlopi_cloud 
    ezlopi_gateway)