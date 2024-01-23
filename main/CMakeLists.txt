file(GLOB_RECURSE SOURCES "*.c*")

idf_component_register(SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES driver ezlopi-core-ezlopi ezlopi-util-trace ezlopi-service-webprov ezlopi-core-ble ezlopi-service-timer ezlopi-service-gpioisr ezlopi-service-ble 
    led_strip ezlopi-service-ota ezlopi-core-scenes ezlopi-service-meshbot ezlopi-service-uart ezlopi-sntp)

