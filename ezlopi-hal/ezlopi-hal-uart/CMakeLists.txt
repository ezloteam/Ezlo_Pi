
file(GLOB_RECURSE SOURCES "./*.c*")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-nvs
        ezlopi-core-info
)

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES   ${EZLOPI_CORE_COMPONENTS} )

