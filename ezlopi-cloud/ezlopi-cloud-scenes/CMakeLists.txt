file(GLOB_RECURSE src_files "*.c*")

# Define esp-components
set(ESP_COMPONENTS
        cjext
)

# Define ezlopi-components
set(EZLOPI_COMPONENTS
        ezlopi-util-trace
)
# Define core components
set(EZLOPI_CORE_COMPONENTS
            ezlopi-core-factory-info
            ezlopi-core-scenes
            ezlopi-core-http
            ezlopi-core-errors
)
# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS
            ezlopi-cloud-constants
)


idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES
            ${ESP_COMPONENTS}
            ${EZLOPI_COMPONENTS}
            ${EZLOPI_CORE_COMPONENTS}
            ${EZLOPI_CLOUD_COMPONENTS}
            )