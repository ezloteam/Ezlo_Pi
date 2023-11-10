file(GLOB_RECURSE SOURCES "*.c*")

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES driver ezlopi trace web_provisioning ezlopi_ble timer_service gpio_isr_service ezlopi_ble_service 
    led_strip ota_service ezlopi_scenes ezlopi_meshbot_service core_sntp)

