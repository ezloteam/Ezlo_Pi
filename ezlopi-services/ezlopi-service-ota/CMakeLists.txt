# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE provisioning_src "*.c*")

set(EZLOPI_SDK_COMPONENTS
        bootloader_support
)
set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-wifi
        ezlopi-core-processes
)
set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-ota
)
set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-webprov
)


idf_component_register(SRCS  "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_SDK_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS} 
                ${EZLOPI_CLOUD_COMPONENTS} 
                ${EZLOPI_SERVICE_COMPONENTS} 
)
