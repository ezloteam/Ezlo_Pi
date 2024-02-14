file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "ezlopi_cloud_network.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES  ezlopi-core-factory-info ezlopi-util-trace esp_netif ezlopi-core-wifi ezlopi-cloud-constants)