# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE provisioning_src "*.c*")
idf_component_register(SRCS "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES
    esp_common
    frozen
    esp_http_client
    common_components
    esp-tls
    ezlopi_wss
    websocket_client
    esp_hw_support
    ezlopi_factory_info
    devices_common
    ezlopi_wifi
    data
    devices
    scenes
    registeration
    favorite
    gateways
    info
    modes
    trace
    items
    room
    settings
    network
    ezlopi_wss
    ezlopi_http
)