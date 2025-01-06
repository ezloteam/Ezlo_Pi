
file(GLOB_RECURSE SOURCES "./*.c*")

set(ESP_COMPONENTS
        driver
)

set(EZLOPI_CORE_COMPONENTS
    ezlopi-core-nvs
)

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
    )

