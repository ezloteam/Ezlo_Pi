file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES  ezlopi_factory_info trace esp_netif ezlopi_wifi ezlopi-cloud-constants)