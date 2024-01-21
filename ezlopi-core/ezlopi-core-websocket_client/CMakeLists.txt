file(GLOB_RECURSE websocket_src "*.c")

idf_component_register(SRCS "${websocket_src}"
    INCLUDE_DIRS "."
    REQUIRES json nvs_flash esp_websocket_client ezlopi-core-factory_info trace)
