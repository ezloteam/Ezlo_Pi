# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE provisioning_src "*.c*")
idf_component_register(SRCS "web_provisioning.c" "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    esp_common frozen esp_http_client common_components esp-tls esp_hw_support
                ezlopi_factory_info ezlopi_wifi trace ezlopi_websocket_client ezlopi_http 
                data devices scenes registration favorite gateways info modes items room 
                settings network feature firmware json
)