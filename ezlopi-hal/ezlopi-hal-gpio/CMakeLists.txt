
file(GLOB_RECURSE SOURCES "./*.c*")

set(ESP_COMPONENTS
        driver
        esp_adc_cal
)
idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_COMPONENTS}
)

