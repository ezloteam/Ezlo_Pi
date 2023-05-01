file(GLOB_RECURSE SOURCES "*.c*")

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES driver ezlopi trace web_provisioning ezlopi_ble timer_service gpio_isr_service ezlopi_ble_service 
    bme680 led_strip ota_service mac_uuid)

