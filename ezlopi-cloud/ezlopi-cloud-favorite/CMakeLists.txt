file(GLOB_RECURSE dir_srcs "*.c*")
idf_component_register(SRCS "${dir_srcs}"
    INCLUDE_DIRS "."
    REQUIRES    esp_common esp_http_client esp-tls esp_hw_support
                ezlopi-core-factory_info ezlopi-core-wifi trace ezlopi-core-websocket_client ezlopi-core-http 
                ezlopi-cloud-data ezlopi-cloud-devices ezlopi-cloud-scenes ezlopi-cloud-registration ezlopi-cloud-favorite ezlopi-cloud-gateways ezlopi-cloud-info ezlopi-cloud-modes ezlopi-cloud-items ezlopi-cloud-room 
                ezlopi-cloud-settings ezlopi-cloud-network ezlopi-cloud-ota json
)
