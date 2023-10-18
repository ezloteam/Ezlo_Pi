idf_component_register(SRCS "servo_motor_MG_996R.c"
                        INCLUDE_DIRS .
                        REQUIRES trace library ezlopi_actions ezlopi_hal ezlopi_timer ezlopi_devices ezlopi_cloud_constants gpio_isr_service)