# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE provisioning_src "*.c*")
idf_component_register(SRCS "web_provisioning.c" "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    data
                devices
                favorite
                firmware
                gateways
                info
                json
                modes
                network
                registration
                room
                scenes
                trace  
                items    
                settings         
                esp_common
                esp_hw_support
                esp_http_client
                esp-tls
                ezlopi_factory_info
                ezlopi_http
                ezlopi_websocket_client
                ezlopi_wifi
            

)
