

idf_component_register(SRCS "sensor_0067_hilink_presence_sensor.c" "hilink_presence_sensor_setting"
                        INCLUDE_DIRS .
                        REQUIRES trace ezlopi_actions ezlopi_hal ezlopi_timer ezlopi_devices ezlopi_cloud_constants gpio_isr_service ezlopi_cloud ld2410)

