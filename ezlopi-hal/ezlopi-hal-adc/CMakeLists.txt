
file(GLOB_RECURSE SOURCES "./*.c*")

set(ESP_COMPONENTS
        esp_adc_cal
)
set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-nvs   
        ezlopi-core-errors     
)

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}            
    )

