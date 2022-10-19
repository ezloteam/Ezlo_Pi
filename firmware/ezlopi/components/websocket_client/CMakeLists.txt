file(GLOB_RECURSE websocket_src "src/*.c")

idf_component_register(SRCS "${websocket_src}"
    INCLUDE_DIRS "include"
    REQUIRES frozen nvs_flash common_components esp_websocket_client ezlopi_factory_info trace)