# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-wifi
        ezlopi-core-modes
        ezlopi-core-sntp
        ezlopi-util-trace
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
        ezlopi-cloud-coordinates
)

idf_component_register(SRCS "ezlopi_core_ezlopi.c" "ezlopi_core_ezlopi_broadcast.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS}
)

