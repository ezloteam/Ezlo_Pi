file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES  ezlopi-core-factory-info ezlopi-util-trace esp_netif ezlopi-core-wifi ezlopi-cloud-constants)