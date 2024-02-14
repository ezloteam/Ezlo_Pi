# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_UTIL_COMPONENTS 
        ezlopi-util-trace 
)

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-wifi
        ezlopi-core-sntp
        ezlopi-core-ble
        ezlopi-core-scenes       

)

set(EZLOPI_CLOUD_COMPONENTS 

)

set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-webprov 
        ezlopi-service-timer 
        ezlopi-service-gpioisr 
        ezlopi-service-ble 
        ezlopi-service-ota                 
        ezlopi-service-uart
        ezlopi-service-meshbot
        ezlopi-service-modes

)



idf_component_register(SRCS "ezlopi_core_ezlopi.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                ezlopi-user-config
                ${EZLOPI_UTIL_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS}
                ${EZLOPI_SERVICE_COMPONENTS}
                )
