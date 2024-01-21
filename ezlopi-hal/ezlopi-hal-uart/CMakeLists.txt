
file(GLOB_RECURSE SOURCES "./*.c*")

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES trace esp_adc_cal ezlopi-core-nvs driver esp_hw_support)

