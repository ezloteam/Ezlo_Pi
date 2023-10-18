idf_component_register(
    SRCS src/led_strip.c
    INCLUDE_DIRS "include"
    REQUIRES driver log color
    PRIV_REQUIRES freertos
)
