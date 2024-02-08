
file(GLOB_RECURSE provisioning_src "*.c*")

set(ESP_IDF_COMPONENTS  
        freertos
    )

set(EZLOPI_COMPONENTS 
        ezlopi-util-trace
    )

set(EZLOPI_CORE_COMPONENTS  
        ezlopi-core-factory-info 
        ezlopi-core-wifi
        ezlopi-core-websocket_client 
        ezlopi-core-http
       
)

set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-data 
        ezlopi-cloud-devices 
        ezlopi-cloud-scenes 
        ezlopi-cloud-registration 
        ezlopi-cloud-favorite 
        ezlopi-cloud-gateways 
        ezlopi-cloud-info 
        ezlopi-cloud-modes 
        ezlopi-cloud-items 
        ezlopi-cloud-room 
        ezlopi-cloud-settings 
        ezlopi-cloud-network 
        ezlopi-cloud-ota
        ezlopi-cloud-location
)



idf_component_register(SRCS "ezlopi_service_webprov.c" "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_IDF_COMPONENTS} 
                ${EZLOPI_COMPONENTS} 
                ${EZLOPI_CORE_COMPONENTS} 
                ${EZLOPI_CLOUD_COMPONENTS} 
    )
