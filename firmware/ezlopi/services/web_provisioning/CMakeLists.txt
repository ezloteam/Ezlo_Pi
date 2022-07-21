# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*")
idf_component_register(SRCS "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES
    esp_common
    frozen
    esp_http_client
    common_components
    esp-tls
    websocket_client
    esp_hw_support
    factory_info
    devices_common
    wifi
    data
    devices
    scenes
    registeration
    favorite
    gateways
    info
    modes
    debug
    items
    room
    settings
)