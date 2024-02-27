    
file(GLOB_RECURSE provisioning_src "*.c*")

set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)

set(SDK_COMPONENTS
)

set(EZLOPI_CORE_COMPONENTS  
        ezlopi-core-ezlopi
)

set(EZLOPI_CLOUD_COMPONENTS 
        
)



idf_component_register(SRCS "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${SDK_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS} 
                ${EZLOPI_UTIL_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS} 
    )
