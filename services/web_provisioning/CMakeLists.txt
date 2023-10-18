# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE provisioning_src "*.c*")
idf_component_register(SRCS "web_provisioning.c" "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    
                trace
                json
                favorite
                gateways
                info
                modes
                items
                room
                data
                devices
                scenes
                registration                
                network
                firmware
                esp_common 
                esp_http_client 
                esp-tls 
                esp_hw_support
                ezlopi_factory_info 
                ezlopi_wifi 
                ezlopi_websocket_client 
                ezlopi_http                             
)