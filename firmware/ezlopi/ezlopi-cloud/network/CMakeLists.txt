file(GLOB_RECURSE src_files "*.c*")
idf_component_register(SRCS "network.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES frozen ezlopi_factory_info trace devices_common esp_netif ezlopi_wifi)