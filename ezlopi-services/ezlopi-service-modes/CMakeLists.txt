# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "./*.c*")

set(ESP_IDF_COMPONENTS 
        freertos
    )
set(EZLOPI_COMPONENTS 
        ezlopi-util-trace
    )
set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-devices 
        ezlopi-core-scenes
        ezlopi-core-processes
        ezlopi-core-modes
        ezlopi-cloud-modes
    )


idf_component_register(SRCS  "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS} 
                ${EZLOPI_COMPONENTS} 
                ${EZLOPI_CORE_COMPONENTS} 
 )