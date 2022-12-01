# file(GLOB_RECURSE src_files "*.c*")

# idf_component_register(SRCS "ble_app.c" "ble_server.c" "ble_server_gap.c" "ble_config.c" "${src_files}"
#     INCLUDE_DIRS "."
#     REQUIRES json ezlopi_factory_info ezlopi_wifi ezlopi_nvs bt nvs_flash)