file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "network.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES frozen ezlopi_factory_info trace esp_netif ezlopi_wifi ezlopi_cloud_constants)