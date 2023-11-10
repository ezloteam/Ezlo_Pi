file(GLOB_RECURSE websocket_src "*.c")

idf_component_register(SRCS "ezlopi_websocket_client.c" "${websocket_src}"
    INCLUDE_DIRS "."
    REQUIRES json nvs_flash esp_websocket_client ezlopi_factory_info trace)
